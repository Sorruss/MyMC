#include "VAO.h"

VAO::VAO()
{
	glGenVertexArrays(1, &ID);
}

void VAO::Bind() const
{
	glBindVertexArray(ID);
}

void VAO::Unbind() const
{
	glBindVertexArray(0);
}

void VAO::Delete() const
{
	glDeleteVertexArrays(1, &ID);
}

void VAO::LinkAttrib(GLuint index, GLint elements, GLenum type, GLsizei stride, const void* offset)
{
	glVertexAttribPointer(index, elements, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(index);
}
