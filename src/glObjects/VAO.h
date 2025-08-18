#pragma once

#include "GLAD/glad.h"

class VAO
{
public:
	VAO();

	void Bind() const;
	void Unbind() const;
	void Delete() const;

	void LinkAttrib(GLuint index, GLint elements, GLenum type, GLsizei stride, const void* offset);

public:
	GLuint ID;
};
