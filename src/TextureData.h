#pragma once

#include "GLAD/glad.h"
#include <vector>
#include <array>
#include <unordered_map>
#include "Vertex.h"

enum CubeType
{
	DIRT,
	GRASS,
	EMPTY,
};

enum Side
{
	FRONT = 0,
	BACK,
	LEFT,
	RIGHT,
	BOTTOM,
	TOP,
};

class TextureData
{
public:
	static std::vector<GLfloat> GetVerticesRaw(const CubeType& type = CubeType::EMPTY);
	static std::vector<GLuint> GetIndices();
	static std::vector<Vertex> GetVerticesFormatted(const CubeType& type = CubeType::EMPTY);
	
	static void SetTextureSize(unsigned size);

private:
	TextureData() {};

	static void LocationToCoords(std::vector<GLfloat>& vertices, const CubeType& type);

private:
	static unsigned TextureSize;
	static const std::unordered_map<CubeType, std::unordered_map<Side, std::array<GLuint, 2>>> TextureLocations;
};
