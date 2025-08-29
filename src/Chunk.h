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

struct Buffers
{
	VAO vao;
	VBO vbo;
	EBO ebo;
};

class Chunk
{
public:
	Chunk(const glm::vec2& position, unsigned size = 16u);
	~Chunk();

	void Render(const ShaderProgram& shader, const Camera& camera, const glm::mat4& proj);
	void Delete();

	std::pair<int, int> getKey() const;
	void GenerateData();
	void GenerateOpenGLData();

private:
	float* const GenChunk();
	void GenBlocks(float* const heightMap);
	void GenBuffers(const CubeType& type);
	void GenAllBuffers();
	void GenFaces();
	void AddFace(const CubeType& type, const Cube& cube, const Side& side);

	void AddVertices(const CubeType& type, const std::vector<Vertex>& vertices);
	void AddIndices(const CubeType& type, unsigned faces = 6);

	void BindTextures() const;
	void UnbindTextures() const;
	void DeleteTextures() const;

	void DeleteBuffers() const;

private:
	// Positioning.
	glm::vec3 Position;
	glm::vec2 ChunkPosition;

	unsigned Size_X = 16u;
	unsigned Size_Y = 32;
	unsigned Size_Z = 16u;

	// Naive Meshing.
	Array3D<Cube> Cubes;
	std::unordered_set<Texture2D, Texture2D::Hash> Textures;

	// Cubes Data.
	std::unordered_map<CubeType, std::vector<Vertex>> blockTypeVertices;
	std::unordered_map<CubeType, std::vector<GLuint>> blockTypeIndices;
	std::unordered_map<CubeType, Material> blockTypeMaterial;

	// Buffers.
	std::unordered_map<CubeType, Buffers> buffers;

	// Perlin Noise.
	const siv::PerlinNoise::seed_type seed = 1234567890u;
	const siv::PerlinNoise perlinNoise{ seed };
};
