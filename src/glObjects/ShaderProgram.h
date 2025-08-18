#pragma once

#include "GLAD/glad.h"
#include <iostream>
#include <format>
#include "glm/gtc/matrix_transform.hpp"

#include "../helpers.h"

#define DIRECTIONAL_LIGHT 0x00000001
#define	POINT_LIGHT 0x00000002
#define SPOT_LIGHT 0x00000004
#define NO_LIGHT 0x00000008

struct Material
{
	Material() : diffuse(0), specular(0), emissive(0)
	{}

	GLuint diffuse;
	GLuint specular;
	GLuint emissive;
	float shininess = 32.0f;
	glm::vec3 tint = glm::vec3(1.0f);

	friend std::ostream& operator<<(std::ostream& stream, const Material& material)
	{
		stream << "diffuse: " << material.diffuse
			<< ", specular: " << material.specular << ", emissive: " << material.emissive
			<< ", shininess: " << material.shininess;
		return stream;
	}
};

struct DirectLight
{
	DirectLight(const glm::vec3& _direction, const glm::vec3& _ambient, const glm::vec3& _diffuse,
		const glm::vec3& _specular) : direction(_direction), ambient(_ambient), diffuse(_diffuse),
		specular(_specular)
	{}

	glm::vec3 direction;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct PointLight
{
	PointLight(const glm::vec3& _position, const glm::vec3& _ambient, const glm::vec3& _diffuse,
		const glm::vec3& _specular, float _attenConstant, float _attenLinear, float _attenQuadratic)
		: position(_position), ambient(_ambient), diffuse(_diffuse), specular(_specular),
		attenConstant(_attenConstant), attenLinear(_attenLinear), attenQuadratic(_attenQuadratic)
	{}

	glm::vec3 position;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float attenConstant;
	float attenLinear;
	float attenQuadratic;
};

struct SpotLight
{
	SpotLight(const glm::vec3& _position, const glm::vec3& _direction, const glm::vec3& _ambient,
		const glm::vec3& _diffuse, const glm::vec3& _specular, float _innerCutOff, float _outerCutOff,
		float _attenConstant, float _attenLinear, float _attenQuadratic)
		: position(_position), direction(_direction), ambient(_ambient), diffuse(_diffuse), specular(_specular),
		innerCutOff(_innerCutOff), outerCutOff(_outerCutOff), attenConstant(_attenConstant), attenLinear(_attenLinear),
		attenQuadratic(_attenQuadratic)
	{}

	glm::vec3 position;
	glm::vec3 direction;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float innerCutOff;
	float outerCutOff;

	float attenConstant;
	float attenLinear;
	float attenQuadratic;
};

class ShaderProgram
{
public:
	ShaderProgram();
	ShaderProgram(const char* vertexSourcePath, const char* fragmentSourcePath);

	ShaderProgram Bind() const;
	void Unbind() const;
	void Delete() const;

	void Init(const char* vertexSourcePath, const char* fragmentSourcePath);

	void BindUniform1i(const char* name, GLint value) const;
	void BindUniform1f(const char* name, GLfloat value) const;
	void BindUniformMat4(const char* name, const GLfloat* value) const;
	void BindUniformVec3(const char* name, float x, float y, float z) const;
	void BindUniformVec3(const char* name, const glm::vec3& vec) const;

	void BindDirectLight(const DirectLight& light) const;
	void BindPointLight(const PointLight& light, int index) const;
	void BindSpotLight(const SpotLight& light) const;

	void BindMaterial(const Material& material) const;

	void SetLights(int value) const;

private:
	void CheckShaderStates(GLuint shader, const char* shaderName);
	void CheckProgramStates(GLuint program);

public:
	GLuint ID;
};
