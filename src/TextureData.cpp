#include "TextureData.h"

unsigned TextureData::TextureSize = 16;
const std::unordered_map<CubeType, std::unordered_map<Side, std::array<GLuint, 2>>> TextureData::TextureLocations =
{
	{
		CubeType::DIRT,
		{
			{ Side::FRONT,  {{ 2, 15 }} },
			{ Side::BACK,   {{ 2, 15 }} },
			{ Side::LEFT,   {{ 2, 15 }} },
			{ Side::RIGHT,  {{ 2, 15 }} },
			{ Side::BOTTOM, {{ 2, 15 }} },
			{ Side::TOP,    {{ 2, 15 }} },
		}
	},
	{
		CubeType::GRASS,
		{
			{ Side::FRONT,  {{ 3, 15 }} },
			{ Side::BACK,   {{ 3, 15 }} },
			{ Side::LEFT,   {{ 3, 15 }} },
			{ Side::RIGHT,  {{ 3, 15 }} },
			{ Side::BOTTOM, {{ 2, 15 }} },
			{ Side::TOP,    {{ 8, 13 }} },
		}
	}
};

std::vector<GLfloat> TextureData::GetVerticesRaw(const CubeType& type)
{
	std::vector<GLfloat> result = {
		// Positions		 // Texture   // Normals
		-0.5f,  0.5f,  0.5f, 0.0f, 1.0f,  0.0f, 0.0f, -1.0f, // FRONT
		 0.5f,  0.5f,  0.5f, 1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
		 0.5f, -0.5f,  0.5f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,

		 0.5f,  0.5f, -0.5f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f, // BACK
		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, // LEFT
		-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

		 0.5f,  0.5f,  0.5f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f, // RIGHT
		 0.5f,  0.5f, -0.5f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f, 0.0f, 1.0f,  0.0f, -1.0f, 0.0f, // BOTTOM
		 0.5f, -0.5f,  0.5f, 1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f,

		-0.5f,  0.5f, -0.5f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f, // TOP
		 0.5f,  0.5f, -0.5f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
	};
	LocationToCoords(result, type);
	return result;
}

std::vector<GLuint> TextureData::GetIndices()
{
	std::vector<GLuint> result = {
		0, 1, 2,
		2, 3, 0,

		4, 5, 6,
		6, 7, 4,

		8, 9, 10,
		10, 11, 8,

		12, 13, 14,
		14, 15, 12,

		16, 17, 18,
		18, 19, 16,

		20, 21, 22,
		22, 23, 20,
	};
	return result;
}

std::vector<Vertex> TextureData::GetVerticesFormatted(const CubeType& type)
{
	std::vector<Vertex> result;
	result.reserve(24);
	std::vector<GLfloat> vertices = GetVerticesRaw(type);
	for (int i{}; i < vertices.size(); i += 8)
	{
		result.emplace_back(
			vertices[i], vertices[i + 1], vertices[i + 2],
			vertices[i + 3], vertices[i + 4],
			vertices[i + 5], vertices[i + 6], vertices[i + 7]);
	}
	return result;
}

void TextureData::SetTextureSize(unsigned size)
{
	TextureData::TextureSize = size;
}

void TextureData::LocationToCoords(std::vector<GLfloat>& vertices, const CubeType& type)
{
	for (int i{}; i < 6; ++i)
	{
		std::array<GLuint, 2> location = TextureLocations.at(type).at(static_cast<Side>(i));
		float x = static_cast<float>(location[0]) / TextureSize;
		float y = static_cast<float>(location[1]) / TextureSize;
		float one = 1.0f / TextureSize;

		vertices[(i * 32) + (0 * 8) + 3] = x; // top-left-x
		vertices[(i * 32) + (0 * 8) + 4] = y + one; // top-left-y

		vertices[(i * 32) + (1 * 8) + 3] = x + one; // top-right-x
		vertices[(i * 32) + (1 * 8) + 4] = y + one; // top-right-y

		vertices[(i * 32) + (2 * 8) + 3] = x + one; // bottom-right-x
		vertices[(i * 32) + (2 * 8) + 4] = y; // bottom-right-y

		vertices[(i * 32) + (3 * 8) + 3] = x; // bottom-left-x
		vertices[(i * 32) + (3 * 8) + 4] = y; // bottom-left-y
	}
}
