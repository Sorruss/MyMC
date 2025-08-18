#include "Texture2D.h"

Texture2D::Texture2D() : ID(0), unit(0)
{
}

Texture2D::Texture2D(const char* path, int unit, GLint format, GLint wraps, GLint wrapt) : unit(unit)
{
	int width, height, channels;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
	if (!data)
	{
		PrintError("Couldn't load the texture source.");
	}

	glGenTextures(1, &ID);
	Bind();

	GLuint internalFormat = format;
	if (format == -1)
	{
		if (channels == 1)
		{
			format = GL_RED;
			internalFormat = GL_RED;

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ONE);
		}
		else if (channels == 3)
		{
			format = GL_RGB;
			internalFormat = GL_RGB;
		}
		else if (channels == 4)
		{
			format = GL_RGBA;
			internalFormat = GL_RGBA;
		}
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wraps);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapt);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	//glGenerateMipmap(GL_TEXTURE_2D);
	Unbind();

	stbi_image_free(data);
}

void Texture2D::Bind() const
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture2D::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Delete() const
{
	glDeleteTextures(1, &ID);
}
