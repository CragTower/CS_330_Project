#include "VBO.h"

// Constructor
VBO::VBO(std::vector<Vertex>& verts)
{
	// Generates new VBO with unique ID
	glGenBuffers(1, &ID);
	// Binds the buffer to make it current
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	// Sends the data to GPU
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), &verts[0], GL_STATIC_DRAW);
}

// Binds VBO to make it current
void VBO::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

// Unbinds VBO by binding to 0 so not to modify any other VBOs
void VBO::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Deletes VBO
void VBO::Delete()
{
	glDeleteBuffers(1, &ID);
}