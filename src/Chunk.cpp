#include "Chunk.h"

Chunk::Chunk(const glm::vec2& position)
	: Position(position.x, position.y, 0.0f)
{
	// Temporary
	material = Material();
	
	Cubes.Init(Size_X, Size_Z, Size_Y);
	float* const heightMap = GenChunk();
	
	GenBlocks(heightMap);
	delete[] heightMap;
	GenFaces();
	Cubes.Delete();
	GenBuffers();
}

void Chunk::Render(const ShaderProgram& shader, const Camera& camera, const glm::mat4& proj)
{
	shader.Bind();
	shader.SetLights(NO_LIGHT);
	shader.BindUniformVec3("viewPos", camera.pos);

	// Material Uniform.
	BindTextures();
	shader.BindMaterial(material);

	// MVP uniform.
	shader.BindUniformMat4("view", glm::value_ptr(camera.view));
	shader.BindUniformMat4("projection", glm::value_ptr(proj));
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, Position);
	shader.BindUniformMat4("model", glm::value_ptr(model));

	chunkVAO.Bind();
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(blocksIndices.size()), GL_UNSIGNED_INT, 0);

	shader.Unbind();
	chunkVAO.Unbind();
	UnbindTextures();
}

void Chunk::Delete()
{
	chunkEBO.Delete();
	chunkVBO.Delete();
	chunkVAO.Delete();
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
			double uniqueX = Position.x + x * scale;
			double uniqueZ = Position.z + z * scale;

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

void Chunk::AddVertices(const std::vector<Vertex>& vertices)
{
	for (auto& vertex : vertices)
	{
		blocksVertices.emplace_back(vertex);
	}
}

void Chunk::AddIndices(unsigned count, unsigned faces)
{
	unsigned offset = count * faces * 4;
	for (unsigned i{}; i < faces; ++i)
	{
		blocksIndices.emplace_back(offset);
		blocksIndices.emplace_back(offset + 1);
		blocksIndices.emplace_back(offset + 2);
		blocksIndices.emplace_back(offset + 2);
		blocksIndices.emplace_back(offset + 3);
		blocksIndices.emplace_back(offset);

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

void Chunk::GenBuffers()
{
	const unsigned size = static_cast<unsigned>(blocksVertices.size()) * 8;
	GLfloat* vertices = new GLfloat[size];

	int vertexCount = 0;
	for (const Vertex& vertex : blocksVertices)
	{
		memcpy(&vertices[vertexCount], vertex.GetRaw().data(), sizeof(GLfloat) * 8);
		vertexCount += 8;
	}

	chunkVAO = VAO();
	chunkVAO.Bind();
	chunkVBO = VBO(vertices, size * sizeof(GLfloat), GL_STATIC_DRAW);
	chunkVAO.LinkAttrib(0, 3, GL_FLOAT, sizeof(GLfloat) * 8, (void*)0);
	chunkVAO.LinkAttrib(1, 2, GL_FLOAT, sizeof(GLfloat) * 8, (void*)(sizeof(GLfloat) * 3));
	chunkVAO.LinkAttrib(2, 3, GL_FLOAT, sizeof(GLfloat) * 8, (void*)(sizeof(GLfloat) * 5));
	chunkEBO = EBO(blocksIndices.data(), blocksIndices.size() * sizeof(GLuint), GL_STATIC_DRAW);

	chunkVAO.Unbind();
	chunkVBO.Unbind();
	chunkEBO.Unbind();
	delete[] vertices;
}

void Chunk::GenFaces()
{
	unsigned cubeIndex = 0;
	for (int x{}; x < Size_X; ++x)
	{
		for (int z{}; z < Size_Z; ++z)
		{
			for (int y{}; y < Size_Y; ++y)
			{
				if (Cubes.at(x, z, y).Type() == CubeType::EMPTY)
				{
					continue;
				}
				Textures.insert(Cubes.at(x, z, y).Texture());

				// Right & Left.
				if (x == Size_X - 1 || Cubes.at(x + 1, z, y).Type() == CubeType::EMPTY)
				{
					AddVertices(Cubes.at(x, z, y).GetVerticesSide(Side::RIGHT));
					AddIndices(cubeIndex++, 1);
				}
				if (x == 0 || Cubes.at(x - 1, z, y).Type() == CubeType::EMPTY)
				{
					AddVertices(Cubes.at(x, z, y).GetVerticesSide(Side::LEFT));
					AddIndices(cubeIndex++, 1);
				}
				// Back & Front.
				if (z == Size_Z - 1 || Cubes.at(x, z + 1, y).Type() == CubeType::EMPTY)
				{
					AddVertices(Cubes.at(x, z, y).GetVerticesSide(Side::FRONT));
					AddIndices(cubeIndex++, 1);
				}
				if (z == 0 || Cubes.at(x, z - 1, y).Type() == CubeType::EMPTY)
				{
					AddVertices(Cubes.at(x, z, y).GetVerticesSide(Side::BACK));
					AddIndices(cubeIndex++, 1);
				}
				// Top & Bottom.
				if (y == Size_Y - 1 || Cubes.at(x, z, y + 1).Type() == CubeType::EMPTY)
				{
					AddVertices(Cubes.at(x, z, y).GetVerticesSide(Side::TOP));
					AddIndices(cubeIndex++, 1);
				}
				if (1)
				{
					if (y == 0 || Cubes.at(x, z, y - 1).Type() == CubeType::EMPTY)
					{
						AddVertices(Cubes.at(x, z, y).GetVerticesSide(Side::BOTTOM));
						AddIndices(cubeIndex++, 1);
					}
				}
			}
		}
	}
}
