#include "GameObject.h"

std::vector<Vertex> GameObject::GetVertices() const
{
	return Vertices;
}

void GameObject::MoveVertices(const glm::vec3& offset)
{
	for (Vertex& vertex : Vertices)
	{
		vertex.Position += offset;
	}
}
