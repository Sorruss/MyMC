#pragma once

#include "glm/gtc/matrix_transform.hpp"
#include "glObjects/ShaderProgram.h"
#include "glObjects/Camera.h"
#include "Vertex.h"
#include <vector>

class GameObject
{
public:
	virtual void Draw(const ShaderProgram& shader, const Camera& camera, const glm::mat4& proj) const = 0;

	virtual std::vector<Vertex> GetVertices() const;
	virtual void MoveVertices(const glm::vec3& offset);

public:
	// Vertices.
	std::vector<Vertex> Vertices;

	// Properties.
	glm::vec3 Position, Rotation, Scale;
	glm::vec3 Velocity;
};
