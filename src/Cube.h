#pragma once

#include "glObjects/VAO.h"
#include "glObjects/VBO.h"
#include "glObjects/EBO.h"
#include "glObjects/Texture2D.h"
#include "glObjects/ShaderProgram.h"
#include "glm/gtc/type_ptr.hpp"
#include "ResourceManager.h"
#include "GameObject.h"
#include "TextureData.h"

class Cube : public GameObject
{
public:
	Cube(const CubeType& type = CubeType::EMPTY);
	Cube(const CubeType& type, const glm::vec3& position);
	~Cube();

	void Draw(const ShaderProgram& shader, const Camera& camera, const glm::mat4& proj) const override;
	std::vector<Vertex> GetVerticesSide(const Side& side) const;

	CubeType Type() const;
	Texture2D Texture() const;
	Material GetMaterial() const;

private:
	void InitializeType();
	void InitializeData();

private:
	// Properties.
	CubeType type;
	Material material;
	
	// Textures.
	Texture2D atlas;
};
