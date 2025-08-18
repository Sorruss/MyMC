#pragma once

#include "glm/glm.hpp"
#include <vector>
#include "glObjects/VAO.h"
#include "glObjects/VBO.h"
#include "glObjects/EBO.h"
#include "Vertex.h"
#include "Cube.h"
#include "PerlinNoise/PerlinNoise.hpp"
#include "Array3D.h"
#include <unordered_set>

class Chunk
{
public:
	Chunk(const glm::vec2& position);

	void Render(const ShaderProgram& shader, const Camera& camera, const glm::mat4& proj);
	void Delete();

private:
	float* const GenChunk();
	void GenBlocks(float* const heightMap);
	void GenBuffers();
	void GenFaces();

	void AddVertices(const std::vector<Vertex>& vertices);
	void AddIndices(unsigned count, unsigned faces = 6);

	void BindTextures() const;
	void UnbindTextures() const;
	void DeleteTextures() const;

private:
	// Positioning.
	glm::vec3 Position;

	const unsigned Size_X = 16;
	unsigned Size_Y = 16;
	const unsigned Size_Z = 16;

	// Naive Meshing.
	Array3D<Cube> Cubes;
	std::unordered_set<Texture2D, Texture2D::Hash> Textures;

	// Vertices.
	std::vector<Vertex> blocksVertices;
	std::vector<GLuint> blocksIndices;

	// Buffers.
	VAO chunkVAO;
	VBO chunkVBO;
	EBO chunkEBO;

	// Perlin Noise.
	const siv::PerlinNoise::seed_type seed = 1234567890u;
	const siv::PerlinNoise perlinNoise{ seed };

	// Temporary.
	Material material;
};
