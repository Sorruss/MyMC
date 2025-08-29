#include "Cube.h"

Cube::Cube(const CubeType& type) : type(type)
{
	Position = glm::vec3(0.0f);

	InitializeType();
	InitializeData();
}

Cube::Cube(const CubeType& type, const glm::vec3& position) : type(type)
{
	Position = position;

	InitializeType();
	InitializeData();
}

Cube::~Cube()
{
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
	
	// vao.Bind();
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	shader.Unbind();
	// vao.Unbind();
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

Material Cube::GetMaterial() const
{
	return material;
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
		material.tintTop = glm::vec3(0.6f, 0.7f, 0.3f) * 1.3f;
		break;
	default:
		break;
	}
}

void Cube::InitializeData()
{
	if (type == CubeType::EMPTY)
		return;

	std::vector<GLfloat> vertices = TextureData::GetVerticesRaw(type);
	std::vector<GLuint> indices = TextureData::GetIndices();
	Vertices = TextureData::GetVerticesFormatted(type);
}
