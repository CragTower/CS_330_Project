#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex>& verts, std::vector<GLuint>& indices)
{
	// Assign parameter vector values to class values
	Mesh::verts = verts;
	Mesh::indices = indices;
	
	// Binds the VAO to make it current
	VAO.Bind();

	// Creates both the VBO and EBO with parameter vector data
	VBO VBO(verts);
	EBO EBO(indices);

	// Tells GPU the strides between different data: vertex positions, normals, and texture coords
	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	VAO.LinkAttrib(VBO, 1, 4, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	VAO.LinkAttrib(VBO, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
	
	// Unbinds VAO, VBO, and EBO so they can't be modified anymore
	VAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();
}

void Mesh::Draw(Shader& shader, Texture& texture)
{
	// Activates Shader to make it current
	shader.Activate();
	// Activates VAO to make it current
	VAO.Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.ID);
	// Draws VAO data to the back buffer
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	
}

void Mesh::Delete()
{
	// FIXME: Create Delete function for Mesh class
}