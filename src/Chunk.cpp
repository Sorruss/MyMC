#include "Chunk.h"
#include "World.h"

#define TIMER 0

Chunk::Chunk(const glm::vec2& position, unsigned size)
	: ChunkPosition(position), Size_X(size), Size_Z(size)
{
	Position = glm::vec3(position.x * Size_X, 0.0f, position.y * Size_Z);
	Cubes.Init(Size_X, Size_Z, Size_Y);
}

Chunk::~Chunk()
{
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

std::pair<int, int> Chunk::getKey() const
{
	return std::pair<int, int>(ChunkPosition.x, ChunkPosition.y);
}

void Chunk::GenerateData()
{
	float* const heightMap = GenChunk();

	GenBlocks(heightMap);
	delete[] heightMap;
	GenFaces();
	Cubes.Delete();
	GenBuffersData();
}

void Chunk::GenerateOpenGLData()
{
	GenAllBuffers();
	DeleteBuffersData();
}

float* const Chunk::GenChunk()
{
	float* const heightMap = new float[Size_X * Size_Z];
	const float scale = 0.01f;
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
#if TIMER
	Timer timer("GenBlocks");
#endif

	static const Cube grassCube(CubeType::GRASS);
	static const Cube dirtCube(CubeType::DIRT);
	static const Cube emptyCube(CubeType::EMPTY);

	blockTypeMaterial.clear();
	blockTypeMaterial[CubeType::GRASS] = grassCube.GetMaterial();
	blockTypeMaterial[CubeType::DIRT] = dirtCube.GetMaterial();

	for (unsigned x{}; x < Size_X; ++x)
	{
		for (unsigned z{}; z < Size_Z; ++z)
		{
			const int height = static_cast<int>(heightMap[x * Size_Z + z]);
			const float posX = static_cast<float>(x);
			const float posZ = static_cast<float>(z);

			for (unsigned y{}; y < Size_Y; ++y)
			{
				const float posY = static_cast<float>(y);
				if (y == height - 1)
				{
					Cubes.at(x, z, y) = grassCube;
					Cubes.at(x, z, y).MoveVertices(glm::vec3(posX, posY, posZ));
				}
				else if (y < height - 1)
				{
					Cubes.at(x, z, y) = dirtCube;
					Cubes.at(x, z, y).MoveVertices(glm::vec3(posX, posY, posZ));
				}
				else
				{
					Cubes.at(x, z, y) = emptyCube;
				}
			}
		}
	}
}

void Chunk::GenBuffersData()
{
#if TIMER
	Timer timer("GenBuffersData");
#endif

	for (const auto& [type, vertices] : blockTypeVertices)
	{
		const unsigned size = static_cast<unsigned>(vertices.size()) * 8u;
		GLfloat* verts = new GLfloat[size];

		int vertexCount = 0;
		for (const Vertex& vertex : vertices)
		{
			memcpy(&verts[vertexCount], vertex.GetData(), sizeof(GLfloat) * 8);
			vertexCount += 8;
		}

		BuffersData.try_emplace(type, verts, size);
	}
}

void Chunk::AddVertices(const CubeType& type, const std::vector<Vertex>& vertices)
{
	std::vector<Vertex>& verts = blockTypeVertices[type];
	for (const Vertex& vertex : vertices)
	{
		verts.emplace_back(vertex);
	}
}

void Chunk::AddIndices(const CubeType& type, unsigned faces)
{
	unsigned count = blockTypeIndices[type].size() / 6;
	unsigned offset = count * faces * 4;
	std::vector<GLuint>& ids = blockTypeIndices.at(type);
	for (unsigned i{}; i < faces; ++i)
	{
		ids.emplace_back(offset);
		ids.emplace_back(offset + 1);
		ids.emplace_back(offset + 2);
		ids.emplace_back(offset + 2);
		ids.emplace_back(offset + 3);
		ids.emplace_back(offset);

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

void Chunk::DeleteBuffersData()
{
	for (auto& [type, data] : BuffersData)
	{
		delete[] data.data;
	}
	BuffersData.clear();
}

void Chunk::GenBuffers(const CubeType& type)
{
	GLfloat* vertices = BuffersData.at(type).data;
	buffers[type].vao = VAO();
	Buffers& bfs = buffers.at(type);
	bfs.vao.Bind();
	bfs.vbo = VBO(vertices, BuffersData.at(type).size * sizeof(GLfloat), GL_STATIC_DRAW);
	bfs.vao.LinkAttrib(0, 3, GL_FLOAT, sizeof(GLfloat) * 8, (void*)0);
	bfs.vao.LinkAttrib(1, 2, GL_FLOAT, sizeof(GLfloat) * 8, (void*)(sizeof(GLfloat) * 3));
	bfs.vao.LinkAttrib(2, 3, GL_FLOAT, sizeof(GLfloat) * 8, (void*)(sizeof(GLfloat) * 5));
	bfs.ebo = EBO(blockTypeIndices.at(type).data(), blockTypeIndices.at(type).size() * sizeof(GLuint), GL_STATIC_DRAW);

	bfs.vao.Unbind();
	bfs.vbo.Unbind();
	bfs.ebo.Unbind();
}

void Chunk::GenAllBuffers()
{
	buffers.clear();
	for (const auto& entry : blockTypeVertices)
	{
		GenBuffers(entry.first);
	}
}

void Chunk::GenFaces()
{
#if TIMER
	Timer timer("GenFaces");
#endif

	for (auto& [type, vertices] : blockTypeVertices) vertices.clear();
	for (auto& [type, indices] : blockTypeIndices) indices.clear();
	Textures.clear();

	Textures.insert(ResourceManager::GetTexture("atlas-1"));

	for (int x{}; x < Size_X; ++x)
	{
		for (int z{}; z < Size_Z; ++z)
		{
			for (int y{}; y < Size_Y; ++y)
			{
				const Cube& cube = Cubes.at(x, z, y);
				const CubeType& type = cube.Type();
				if (type == CubeType::EMPTY)
				{
					continue;
				}
				
				// Right & Left.
				if (x == Size_X - 1 || Cubes.at(x + 1, z, y).Type() == CubeType::EMPTY)
				{
					AddFace(type, cube, Side::RIGHT);
				}
				if (x == 0 || Cubes.at(x - 1, z, y).Type() == CubeType::EMPTY)
				{
					AddFace(type, cube, Side::LEFT);
				}
				// Back & Front.
				if (z == Size_Z - 1 || Cubes.at(x, z + 1, y).Type() == CubeType::EMPTY)
				{
					AddFace(type, cube, Side::FRONT);
				}
				if (z == 0 || Cubes.at(x, z - 1, y).Type() == CubeType::EMPTY)
				{
					AddFace(type, cube, Side::BACK);
				}
				// Top & Bottom.
				if (y == Size_Y - 1 || Cubes.at(x, z, y + 1).Type() == CubeType::EMPTY)
				{
					AddFace(type, cube, Side::TOP);
				}
				if (0)
				{
					if (y == 0 || Cubes.at(x, z, y - 1).Type() == CubeType::EMPTY)
					{
						AddFace(type, cube, Side::BOTTOM);
					}
				}
			}
		}
	}
}

void Chunk::AddFace(const CubeType& type, const Cube& cube, const Side& side)
{
	const std::vector<Vertex>& vertices = cube.GetVerticesSide(side);
	AddVertices(type, vertices);
	AddIndices(type, 1);
}
