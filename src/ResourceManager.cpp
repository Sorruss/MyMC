#include "ResourceManager.h"

std::unordered_map<std::string, ShaderProgram>	ResourceManager::Shaders;
std::unordered_map<std::string, Texture2D>		ResourceManager::Textures;

Texture2D& ResourceManager::LoadTexture(const std::string& name, const char* filepath, int unit)
{
    if (!doesTextureExist(name))
    {
        Textures[name] = loadTexture(filepath, unit);
    }
    return Textures[name];
}

Texture2D& ResourceManager::GetTexture(const std::string& name)
{
    return Textures[name];
}

ShaderProgram& ResourceManager::LoadShader(const std::string& name, const char* vertexSourcePath, const char* fragmentSourcePath)
{
    if (!doesShaderExist(name))
    {
        Shaders[name] = loadShader(vertexSourcePath, fragmentSourcePath);
    }
    return Shaders[name];
}

ShaderProgram& ResourceManager::GetShader(const std::string& name)
{
    return Shaders[name];
}

void ResourceManager::Clear()
{
    for (auto& entry : Textures)
    {
        entry.second.Delete();
    }

    for (auto& entry : Shaders)
    {
        entry.second.Delete();
    }
}

Texture2D ResourceManager::loadTexture(const char* filepath, int unit)
{
    Texture2D texture(filepath, unit);
    return texture;
}

ShaderProgram ResourceManager::loadShader(const char* vertexSourcePath, const char* fragmentSourcePath)
{
    ShaderProgram shader(vertexSourcePath, fragmentSourcePath);
    return shader;
}

bool ResourceManager::doesTextureExist(const std::string& name)
{
    return Textures.contains(name);
}

bool ResourceManager::doesShaderExist(const std::string& name)
{
    return Shaders.contains(name);
}
