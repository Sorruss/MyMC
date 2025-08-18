#pragma once

#include "GLAD/glad.h"

class EBO
{
public:
	EBO();
	EBO(const void* data, GLsizeiptr size, GLenum usage);

	void Bind() const;
	void Unbind() const;
	void Delete() const;

public:
	GLuint ID;
};
