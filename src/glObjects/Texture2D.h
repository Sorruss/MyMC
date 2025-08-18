#pragma once

#include "GLAD/glad.h"
#include "stb_image/stb_image.h"
#include "../helpers.h"

class Texture2D
{
public:
	Texture2D();
	Texture2D(const char* path, int unit, GLint format = -1, GLint wraps = GL_REPEAT, GLint wrapt = GL_REPEAT);

	void Bind() const;
	void Unbind() const;
	void Delete() const;

	bool operator==(const Texture2D& other) const
	{
		return ID == other.ID;
	}

	struct Hash
	{
		size_t operator()(const Texture2D& texture) const
		{
			return std::hash<GLuint>()(texture.ID);
		}
	};

public:
	GLuint ID;
	int unit;
};
