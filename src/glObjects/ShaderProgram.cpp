#include "ShaderProgram.h"

ShaderProgram::ShaderProgram() : ID(0)
{
}

ShaderProgram::ShaderProgram(const char* vertexSourcePath, const char* fragmentSourcePath) : ID(0)
{
	Init(vertexSourcePath, fragmentSourcePath);
}

ShaderProgram ShaderProgram::Bind() const
{
	glUseProgram(ID);
	return *this;
}

void ShaderProgram::Unbind() const
{
	glUseProgram(0);
}

void ShaderProgram::Delete() const
{
	glDeleteProgram(ID);
}

void ShaderProgram::Init(const char* vertexSourcePath, const char* fragmentSourcePath)
{
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vertexSourceString = GetFileContent(vertexSourcePath);
	std::string fragmentSourceString = GetFileContent(fragmentSourcePath);
	const char* vertexSource = vertexSourceString.c_str();
	const char* fragmentSource = fragmentSourceString.c_str();

	glShaderSource(vertexShader, 1, &vertexSource, 0);
	glShaderSource(fragmentShader, 1, &fragmentSource, 0);

	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	CheckShaderStates(vertexShader, "VERTEX");
	CheckShaderStates(fragmentShader, "FRAGMENT");

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	CheckProgramStates(ID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void ShaderProgram::BindUniform1i(const char* name, GLint value) const
{
	glUniform1i(glGetUniformLocation(ID, name), value);
}

void ShaderProgram::BindUniform1f(const char* name, GLfloat value) const
{
	glUniform1f(glGetUniformLocation(ID, name), value);
}

void ShaderProgram::BindUniformMat4(const char* name, const GLfloat* value) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, value);
}

void ShaderProgram::BindUniformVec3(const char* name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name), x, y, z);
}

void ShaderProgram::BindUniformVec3(const char* name, const glm::vec3& vec) const
{
	BindUniformVec3(name, vec.x, vec.y, vec.z);
}

void ShaderProgram::BindDirectLight(const DirectLight& light) const
{
	BindUniformVec3("dLight.direction", light.direction);
	BindUniformVec3("dLight.ambient", light.ambient);
	BindUniformVec3("dLight.diffuse", light.diffuse);
	BindUniformVec3("dLight.specular", light.specular);
}

void ShaderProgram::BindPointLight(const PointLight& light, int index) const
{
	BindUniformVec3(std::format("pLight[{}].position", index).c_str(), light.position);
	BindUniformVec3(std::format("pLight[{}].ambient", index).c_str(), light.ambient);
	BindUniformVec3(std::format("pLight[{}].diffuse", index).c_str(), light.diffuse);
	BindUniformVec3(std::format("pLight[{}].specular", index).c_str(), light.specular);
	BindUniform1f(std::format("pLight[{}].attenConstant", index).c_str(), light.attenConstant);
	BindUniform1f(std::format("pLight[{}].attenLinear", index).c_str(), light.attenLinear);
	BindUniform1f(std::format("pLight[{}].attenQuadratic", index).c_str(), light.attenQuadratic);
}

void ShaderProgram::BindSpotLight(const SpotLight& light) const
{
	BindUniformVec3("sLight.position", light.position);
	BindUniformVec3("sLight.direction", light.direction);
	BindUniformVec3("sLight.ambient", light.ambient);
	BindUniformVec3("sLight.diffuse", light.diffuse);
	BindUniformVec3("sLight.specular", light.specular);
	BindUniform1f("sLight.attenConstant", light.attenConstant);
	BindUniform1f("sLight.attenLinear", light.attenLinear);
	BindUniform1f("sLight.attenQuadratic", light.attenQuadratic);
	BindUniform1f("sLight.innerCutOff", light.innerCutOff);
	BindUniform1f("sLight.outerCutOff", light.outerCutOff);
}

void ShaderProgram::BindMaterial(const Material& material) const
{
	BindUniform1i("material.diffuse", material.diffuse);
	BindUniform1i("material.specular", material.specular);
	BindUniform1i("material.emissive", material.emissive);
	BindUniform1f("material.shininess", material.shininess);
	BindUniformVec3("material.tintTop", material.tintTop);
}

void ShaderProgram::SetLights(int value) const
{
	BindUniform1i("lightBits", value);
}

void ShaderProgram::CheckShaderStates(GLuint shader, const char* shaderName)
{
	int success;
	char errorInfo[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, 0, errorInfo);
		std::cout << "ERROR::SHADER::" << shaderName << "::COMPILE\n" << errorInfo;
	}
}

void ShaderProgram::CheckProgramStates(GLuint program)
{
	int success;
	char errorInfo[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, 512, 0, errorInfo);
		std::cout << "ERROR::PROGRAM::LINK\n" << errorInfo;
	}
}
