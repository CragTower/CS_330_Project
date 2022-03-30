#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex>& verts, std::vector<GLuint>& indices)
{
	Mesh::verts = verts;
	Mesh::indices = indices;
	
	VAO.Bind();

	VBO VBO(verts);
	EBO EBO(indices);

	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	VAO.LinkAttrib(VBO, 1, 4, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));

	VAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();
}

void Mesh::Draw(Shader& shader)
{
	shader.Activate();
	VAO.Bind();

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	
}

void Mesh::Delete()
{
	// FIXME: Create Delete function for Mesh class
}