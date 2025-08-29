#include "World.h"

World::World(unsigned chunkSize) : ChunkSize(chunkSize), LastPlayerChunkPos(INT_MAX)
{
}

void World::Update(const glm::vec3& playerPos)
{
	const int maxChunksPerFrame = 2;
	int processed = 0;
	while (processed < maxChunksPerFrame) {
		if (Chunk* const chunk = ChunksQueue.tryPop())
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

	glm::vec2 playerChunkPos = World2ChunkCoords(playerPos);
	if (playerChunkPos != LastPlayerChunkPos)
	{
		UnloadDistantChunks(playerChunkPos);
		LoadChunks(playerChunkPos);
		LastPlayerChunkPos = playerChunkPos;
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
	if (CurrentTasksCount >= MaxTasks)
	{
		return;
	}

	for (int x = -RenderDistance; x < RenderDistance; ++x) {
		for (int z = -RenderDistance; z < RenderDistance; ++z) {
			std::pair<int, int> chunkKey = { playerChunkPos.x + x, playerChunkPos.y + z };

			if (Chunks.find(chunkKey) == Chunks.end()) {
				float distance = glm::length(glm::vec2(x, z));
				CurrentTasksCount++;
				Futures.emplace_back(std::async(std::launch::async, [this, chunkKey]() {
					Chunk* const chunk = new Chunk({ chunkKey.first, chunkKey.second });
					chunk->GenerateData();
					ChunksQueue.push(chunk);
					CurrentTasksCount--;
					}));
			}
		}
	}
}

void World::UnloadDistantChunks(const glm::vec2& playerChunkPos)
{
	auto it = Chunks.begin();
	while (it != Chunks.end())
	{
		if (glm::distance(playerChunkPos, { it->first.first, it->first.second }) > RenderDistance)
		{
			delete it->second;
			it = Chunks.erase(it);
		}
		else
		{
			++it;
		}
	}
}
