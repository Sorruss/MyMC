#pragma once

#include "glObjects/ShaderProgram.h"
#include <unordered_map>
#include "glObjects/Texture2D.h"

class ResourceManager
{
public:
	static Texture2D& LoadTexture(const std::string& name, const char* filepath, int unit);
	static Texture2D& GetTexture(const std::string& name);

	static ShaderProgram& LoadShader(const std::string& name, const char* vertexSourcePath, const char* fragmentSourcePath);
	static ShaderProgram& GetShader(const std::string& name);

	static void Clear();

private:
	ResourceManager() {}

	static Texture2D loadTexture(const char* filepath, int unit);
	static ShaderProgram loadShader(const char* vertexSourcePath, const char* fragmentSourcePath);

	static bool doesTextureExist(const std::string& name);
	static bool doesShaderExist(const std::string& name);

public:
	static std::unordered_map<std::string, ShaderProgram> Shaders;
	static std::unordered_map<std::string, Texture2D> Textures;
};
