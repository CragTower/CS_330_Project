#include "VAO.h"

// Constructor
VAO::VAO()
{
	// Generates new VAO with unique ID
	glGenVertexArrays(1, &ID);
}

// Links vertex attrib
void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
{
	VBO.Bind();
	glEnableVertexAttribArray(layout);
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	
	VBO.Unbind();
}

// Binds VAO to make it current
void VAO::Bind()
{
	glBindVertexArray(ID);
}

// Unbinds VAO by binding to 0 so not to modify any other VAOs
void VAO::Unbind()
{
	glBindVertexArray(0);
}

// Deletes VAO
void VAO::Delete()
{
	glDeleteVertexArrays(1, &ID);
}