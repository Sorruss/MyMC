#pragma once

#include "GLAD/glad.h"

class VBO
{
public:
	VBO();
	VBO(const void* data, GLsizeiptr size, GLenum usage);
	
	void Bind() const;
	void Unbind() const;
	void Delete() const;

public:
	GLuint ID;
};
