#include "World.h"

World::World(unsigned chunkSize) : ChunkSize(chunkSize), LastPlayerChunkPos(INT_MAX)
{
}

void World::Update(const glm::vec3& playerPos)
{
	AddReadyChunks();

	glm::vec2 playerChunkPos = World2ChunkCoords(playerPos);
	if (playerChunkPos != LastPlayerChunkPos)
	{
		UnloadDistantChunks(playerChunkPos);
		LoadChunks(playerChunkPos);
		LastPlayerChunkPos = playerChunkPos;
	}

	ProcessChunkQueue();
	CleanupFinishedFutures();
}

void World::AddReadyChunks()
{
	const int maxChunksPerFrame = 2;
	int processed = 0;
	while (processed < maxChunksPerFrame)
	{
		if (Chunk* const chunk = ChunksGenerated.tryPop())
		{
			chunk->GenerateOpenGLData();
			Chunks.emplace(chunk->getKey(), chunk);
			processed++;
		}
		else
		{
			break;
		}
	}
}

void World::ProcessChunkQueue()
{
	while (CurrentTasksCount < MaxTasks && !ChunksToGenerate.empty()) {
		ChunkTask task = ChunksToGenerate.top();
		ChunksToGenerate.pop();

		CurrentTasksCount++;
		Futures.emplace_back(std::async(std::launch::async, [this, task]() {
			Chunk* const chunk = new Chunk({ task.key.first, task.key.second });
			chunk->GenerateData();
			ChunksGenerated.push(chunk);
			CurrentTasksCount--;
			}));
	}
}

void World::Render(const ShaderProgram& shader, const Camera& camera, const glm::mat4& proj)
{
	for (auto& [key, chunk] : Chunks)
	{
		chunk->Render(shader, camera, proj);
	}
}

void World::Delete()
{
	for (auto& [key, chunk] : Chunks)
	{
		chunk->Delete();
		delete chunk;
	}
	Chunks.clear();
}

glm::vec2 World::World2ChunkCoords(const glm::vec3& coords) const
{
	return glm::vec2(static_cast<int>(coords.x / ChunkSize), static_cast<int>(coords.z / ChunkSize));
}

void World::LoadChunks(const glm::vec2& playerChunkPos)
{
	for (int x = -RenderDistance; x < RenderDistance; ++x) {
		for (int z = -RenderDistance; z < RenderDistance; ++z) {
			std::pair<int, int> chunkKey = { playerChunkPos.x + x, playerChunkPos.y + z };

			if (Chunks.find(chunkKey) == Chunks.end() &&
				ChunksWaiting.find(chunkKey) == ChunksWaiting.end()) {

				float distance = glm::length(glm::vec2(x, z));
				ChunksToGenerate.push({ chunkKey, distance });
				ChunksWaiting.insert(chunkKey);
			}
		}
	}

	ProcessChunkQueue();
}

void World::UnloadDistantChunks(const glm::vec2& playerChunkPos)
{
	auto it = Chunks.begin();
	while (it != Chunks.end())
	{
		if (glm::distance(playerChunkPos, { it->first.first, it->first.second }) > RenderDistance * 2)
		{
			ChunksWaiting.erase(it->first);
			delete it->second;
			it = Chunks.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void World::CleanupFinishedFutures()
{
	Futures.erase(
		std::remove_if(Futures.begin(), Futures.end(),
			[](const std::future<void>& f) {
				return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
			}),
		Futures.end());
}
