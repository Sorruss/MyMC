#pragma once

#include "GLAD/glad.h"
#include "glm/glm.hpp"
#include <vector>

struct Vertex
{
	glm::vec3 Position;
	glm::vec2 Texture;
	glm::vec3 Normal;

	Vertex(const glm::vec3& position, const glm::vec2& texture, const glm::vec3& normal)
		: Position(position), Texture(texture), Normal(normal)
	{}

	Vertex(GLfloat px, GLfloat py, GLfloat pz, GLfloat tx, GLfloat ty, GLfloat nx, GLfloat ny, GLfloat nz)
		: Position(px, py, pz), Texture(tx, ty), Normal(nx, ny, nz) {}

	Vertex(const GLfloat* data)
	{
		Position = glm::vec3(data[0], data[1], data[2]);
		Texture = glm::vec2(data[3], data[4]);
		Normal = glm::vec3(data[5], data[6], data[7]);
	}

	const GLfloat* GetData() const
	{
		return &Position.x;
	}

	std::vector<GLfloat> GetRaw() const
	{
		std::vector<GLfloat> raw;
		raw.reserve(8);

		raw.emplace_back(Position.x);
		raw.emplace_back(Position.y);
		raw.emplace_back(Position.z);

		raw.emplace_back(Texture.x);
		raw.emplace_back(Texture.y);

		raw.emplace_back(Normal.x);
		raw.emplace_back(Normal.y);
		raw.emplace_back(Normal.z);

		return raw;
	}
};
