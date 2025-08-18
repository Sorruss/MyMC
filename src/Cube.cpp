#include "Cube.h"

Cube::Cube(const CubeType& type) : vao(), type(type)
{
	Position = glm::vec3(0.0f);

	InitializeType();
	InitializeBuffers();
}

Cube::Cube(const CubeType& type, const glm::vec3& position) : vao(), type(type)
{
	Position = position;

	InitializeType();
	InitializeBuffers();
}

Cube::~Cube()
{
	vao.Delete();
}

void Cube::Draw(const ShaderProgram& shader, const Camera& camera, const glm::mat4& proj) const
{
	if (type == CubeType::EMPTY)
		return;

	shader.Bind();
	shader.SetLights(NO_LIGHT);
	shader.BindUniformVec3("viewPos", camera.pos);

	// Material Uniform.
	atlas.Bind();
	shader.BindMaterial(material);

	// MVP uniform.
	shader.BindUniformMat4("view", glm::value_ptr(camera.view));
	shader.BindUniformMat4("projection", glm::value_ptr(proj));
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, Position);
	shader.BindUniformMat4("model", glm::value_ptr(model));
	
	vao.Bind();
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	shader.Unbind();
	vao.Unbind();
	atlas.Unbind();
}

std::vector<Vertex> Cube::GetVerticesSide(const Side& side) const
{
	std::vector<Vertex> result;
	result.reserve(4);

	int offset = side * 4;
	for (int i = offset; i < offset + 4; ++i)
	{
		result.emplace_back(Vertices[i]);
	}

	return result;
}

CubeType Cube::Type() const
{
	return type;
}

Texture2D Cube::Texture() const
{
	return atlas;
}

void Cube::InitializeType()
{
	if (type == CubeType::EMPTY)
		return;

	atlas = ResourceManager::GetTexture("atlas-1");
	material.diffuse = atlas.unit;
	material.shininess = 32.f;

	switch (type) {
	case GRASS:
		material.tint = glm::vec3(0.6f, 0.7f, 0.3f) * 1.3f;
		break;
	default:
		break;
	}
}

void Cube::InitializeBuffers()
{
	if (type == CubeType::EMPTY)
		return;

	std::vector<GLfloat> vertices = TextureData::GetVerticesRaw(type);
	std::vector<GLuint> indices = TextureData::GetIndices();
	Vertices = TextureData::GetVerticesFormatted(type);

	vao.Bind();
	VBO triangleVBO(vertices.data(), vertices.size() * sizeof(GLfloat), GL_STATIC_DRAW);
	EBO ebo(indices.data(), indices.size() * sizeof(GLuint), GL_STATIC_DRAW);
	vao.LinkAttrib(0, 3, GL_FLOAT, sizeof(GL_FLOAT) * 8, (void*)0);
	vao.LinkAttrib(1, 2, GL_FLOAT, sizeof(GL_FLOAT) * 8, (void*)(sizeof(GL_FLOAT) * 3));
	vao.LinkAttrib(2, 3, GL_FLOAT, sizeof(GL_FLOAT) * 8, (void*)(sizeof(GL_FLOAT) * 5));
	
	vao.Unbind();
	triangleVBO.Unbind();
	ebo.Unbind();
}
