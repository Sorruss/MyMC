#include "Chunk.h"

Chunk::Chunk(const glm::vec2& position)
{
	Position = glm::vec3(position.x * Size_X, 0.0f, position.y * Size_Z);

	Cubes.Init(Size_X, Size_Z, Size_Y);
	float* const heightMap = GenChunk();
	
	GenBlocks(heightMap);
	delete[] heightMap;
	GenFaces();
	Cubes.Delete();
	GenAllBuffers();
	CollectMaterials();
}

void Chunk::Render(const ShaderProgram& shader, const Camera& camera, const glm::mat4& proj)
{
	shader.Bind();
	shader.SetLights(NO_LIGHT);
	shader.BindUniformVec3("viewPos", camera.pos);

	// Material Uniform.
	BindTextures();

	// MVP uniform.
	shader.BindUniformMat4("view", glm::value_ptr(camera.view));
	shader.BindUniformMat4("projection", glm::value_ptr(proj));
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, Position);
	shader.BindUniformMat4("model", glm::value_ptr(model));

	for (const auto& entry : blockTypeVertices)
	{
		buffers.at(entry.first).vao.Bind();
		shader.BindMaterial(blockTypeMaterial.at(entry.first));
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(blockTypeIndices.at(entry.first).size()), GL_UNSIGNED_INT, 0);
		buffers.at(entry.first).vao.Unbind();
	}

	shader.Unbind();
	UnbindTextures();
}

void Chunk::Delete()
{
	DeleteBuffers();
	DeleteTextures();
}

float* const Chunk::GenChunk()
{
	float* const heightMap = new float[Size_X * Size_Z];
	const float scale = 0.05f;
	const int32_t octaves = 3;

	for (unsigned x{}; x < Size_X; ++x)
	{
		for (unsigned z{}; z < Size_Z; ++z)
		{
			double uniqueX = (Position.x + x) * scale;
			double uniqueZ = (Position.z + z) * scale;

			const float perlin = static_cast<float>(perlinNoise.octave2D_01(uniqueX, uniqueZ, octaves));
			heightMap[x * Size_Z + z] = perlin * Size_Y;
		}
	}
	return heightMap;
}

void Chunk::GenBlocks(float* const heightMap)
{
	for (unsigned x{}; x < Size_X; ++x)
	{
		for (unsigned z{}; z < Size_Z; ++z)
		{
			for (float y{}; y < Size_Y; ++y)
			{
				int height = static_cast<int>(heightMap[x * Size_Z + z]);
				if (y == height - 1)
				{
					Cube cube(CubeType::GRASS);
					cube.MoveVertices(glm::vec3(static_cast<float>(x), y, static_cast<float>(z)));
					Cubes.at(x, z, static_cast<int>(y)) = cube;
				}
				else if (y < height - 1)
				{
					Cube cube(CubeType::DIRT);
					cube.MoveVertices(glm::vec3(static_cast<float>(x), y, static_cast<float>(z)));
					Cubes.at(x, z, static_cast<int>(y)) = cube;
				}
				else
				{
					Cube cube(CubeType::EMPTY);
					Cubes.at(x, z, static_cast<int>(y)) = cube;
				}
			}
		}
	}
}

void Chunk::AddVertices(const CubeType& type, const std::vector<Vertex>& vertices)
{
	for (auto& vertex : vertices)
	{
		blockTypeVertices[type].emplace_back(vertex);
	}
}

void Chunk::AddIndices(const CubeType& type, unsigned faces)
{
	unsigned count = blockTypeIndices[type].size() / 6;
	unsigned offset = count * faces * 4;
	for (unsigned i{}; i < faces; ++i)
	{
		blockTypeIndices.at(type).emplace_back(offset);
		blockTypeIndices.at(type).emplace_back(offset + 1);
		blockTypeIndices.at(type).emplace_back(offset + 2);
		blockTypeIndices.at(type).emplace_back(offset + 2);
		blockTypeIndices.at(type).emplace_back(offset + 3);
		blockTypeIndices.at(type).emplace_back(offset);

		offset += 4;
	}
}

void Chunk::BindTextures() const
{
	for (const Texture2D& texture : Textures)
	{
		texture.Bind();
	}
}

void Chunk::UnbindTextures() const
{
	for (const Texture2D& texture : Textures)
	{
		texture.Unbind();
	}
}

void Chunk::DeleteTextures() const
{
	for (const Texture2D& texture : Textures)
	{
		texture.Delete();
	}
}

void Chunk::DeleteBuffers() const
{
	for (const auto& entry : buffers)
	{
		entry.second.ebo.Delete();
		entry.second.vbo.Delete();
		entry.second.vao.Delete();
	}
}

void Chunk::GenBuffers(const CubeType& type)
{
	const unsigned size = static_cast<unsigned>(blockTypeVertices.at(type).size()) * 8u;
	GLfloat* vertices = new GLfloat[size];

	int vertexCount = 0;
	for (const Vertex& vertex : blockTypeVertices.at(type))
	{
		memcpy(&vertices[vertexCount], vertex.GetRaw().data(), sizeof(GLfloat) * 8);
		vertexCount += 8;
	}

	buffers[type].vao = VAO();
	buffers.at(type).vao.Bind();
	buffers.at(type).vbo = VBO(vertices, size * sizeof(GLfloat), GL_STATIC_DRAW);
	buffers.at(type).vao.LinkAttrib(0, 3, GL_FLOAT, sizeof(GLfloat) * 8, (void*)0);
	buffers.at(type).vao.LinkAttrib(1, 2, GL_FLOAT, sizeof(GLfloat) * 8, (void*)(sizeof(GLfloat) * 3));
	buffers.at(type).vao.LinkAttrib(2, 3, GL_FLOAT, sizeof(GLfloat) * 8, (void*)(sizeof(GLfloat) * 5));
	buffers.at(type).ebo = EBO(blockTypeIndices.at(type).data(), blockTypeIndices.at(type).size() * sizeof(GLuint), GL_STATIC_DRAW);

	buffers.at(type).vao.Unbind();
	buffers.at(type).vbo.Unbind();
	buffers.at(type).ebo.Unbind();
	delete[] vertices;
}

void Chunk::GenAllBuffers()
{
	for (const auto& entry : blockTypeVertices)
	{
		GenBuffers(entry.first);
	}
}

void Chunk::GenFaces()
{
	for (int x{}; x < Size_X; ++x)
	{
		for (int z{}; z < Size_Z; ++z)
		{
			for (int y{}; y < Size_Y; ++y)
			{
				CubeType type = Cubes.at(x, z, y).Type();
				if (type == CubeType::EMPTY)
				{
					continue;
				}
				Textures.insert(Cubes.at(x, z, y).Texture());

				// Right & Left.
				if (x == Size_X - 1 || Cubes.at(x + 1, z, y).Type() == CubeType::EMPTY)
				{
					AddVertices(type, Cubes.at(x, z, y).GetVerticesSide(Side::RIGHT));
					AddIndices(type, 1);
				}
				if (x == 0 || Cubes.at(x - 1, z, y).Type() == CubeType::EMPTY)
				{
					AddVertices(type, Cubes.at(x, z, y).GetVerticesSide(Side::LEFT));
					AddIndices(type, 1);
				}
				// Back & Front.
				if (z == Size_Z - 1 || Cubes.at(x, z + 1, y).Type() == CubeType::EMPTY)
				{
					AddVertices(type, Cubes.at(x, z, y).GetVerticesSide(Side::FRONT));
					AddIndices(type, 1);
				}
				if (z == 0 || Cubes.at(x, z - 1, y).Type() == CubeType::EMPTY)
				{
					AddVertices(type, Cubes.at(x, z, y).GetVerticesSide(Side::BACK));
					AddIndices(type, 1);
				}
				// Top & Bottom.
				if (y == Size_Y - 1 || Cubes.at(x, z, y + 1).Type() == CubeType::EMPTY)
				{
					AddVertices(type, Cubes.at(x, z, y).GetVerticesSide(Side::TOP));
					AddIndices(type, 1);
				}
				if (1)
				{
					if (y == 0 || Cubes.at(x, z, y - 1).Type() == CubeType::EMPTY)
					{
						AddVertices(type, Cubes.at(x, z, y).GetVerticesSide(Side::BOTTOM));
						AddIndices(type, 1);
					}
				}
			}
		}
	}
}

void Chunk::CollectMaterials()
{
	for (const auto& entry : blockTypeVertices)
	{
		Cube cube(entry.first);
		blockTypeMaterial[entry.first] = cube.GetMaterial();
	}
}
