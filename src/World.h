#pragma once

#include "Chunk.h"
#include <unordered_map>
#include "glObjects/ShaderProgram.h"
#include <functional>
#include <utility>
#include <future>
#include "Timer.h"
#include <queue>

class ChunkQueue
{
public:
	~ChunkQueue()
	{
		while (!Chunks.empty())
		{
			delete Chunks.front();
			Chunks.pop();
		}
	}

	void push(Chunk* chunk)
	{
		std::lock_guard<std::mutex> lock(Mutex);
		Chunks.push(chunk);
	}

	Chunk* const tryPop()
	{
		std::lock_guard<std::mutex> lock(Mutex);
		if (Chunks.empty())
		{
			return nullptr;
		}

		Chunk* const chunk = Chunks.front();
		Chunks.pop();
		return chunk;
	}

private:
	std::queue<Chunk*> Chunks;
	std::mutex Mutex;
};

struct PairHash
{
	template<class T1, class T2>
	size_t operator()(const std::pair<T1, T2>& other) const
	{
		auto hash1 = std::hash<T1>()(other.first);
		auto hash2 = std::hash<T2>()(other.second);
		return hash1 ^ (hash2 << 1);
	}
};

class World
{
public:
	World(unsigned chunkSize = 16u);

	void Update(const glm::vec3& playerPos);
	void Render(const ShaderProgram& shader, const Camera& camera, const glm::mat4& proj);
	void Delete();

private:
	glm::vec2 World2ChunkCoords(const glm::vec3& coords) const;

	void LoadChunks(const glm::vec2& playerChunkPos);
	void UnloadDistantChunks(const glm::vec2& playerChunkPos);

private:
	std::unordered_map<std::pair<int, int>, Chunk*, PairHash> Chunks;

	float RenderDistance = 5.0f;
	glm::vec2 LastPlayerChunkPos;
	unsigned ChunkSize;

	// async stuff.
	ChunkQueue ChunksQueue;
	std::vector<std::future<void>> Futures;
	std::mutex Mutex;
	const int MaxTasks = 16;
	std::atomic<int> CurrentTasksCount{ 0 };
};
