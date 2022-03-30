#include "EBO.h"

EBO::EBO(std::vector<GLuint>& indices)
{
	// Generate new EBO with unique ID
	glGenBuffers(1, &ID);
	// Binds the EBO to make it current
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	// Sends data to GPU
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
}

// Binds the EBO to make it current
void EBO::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

// Unbinds the EBO by binding it to 0 so not to modify any other EBOs
void EBO::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Deletes EBO
void EBO::Delete()
{
	glDeleteBuffers(1, &ID);
}