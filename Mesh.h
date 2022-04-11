#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include "VAO.h"
#include "EBO.h"
#include "Shader.h"
#include "Texture.h"

class Mesh
{
	public:
		// Public variable to hold incoming vertex vector
		std::vector<Vertex> verts;
		// Public variable to hold incoming index vector
		std::vector<GLuint> indices;
		// Creates new VAO object
		VAO VAO;

		// Constructor
		Mesh(std::vector<Vertex>& verts, std::vector<GLuint>& indices);

		// Draws object
		void Draw(Shader& shader, Texture& texture);
		// Deletes Mesh
		void Delete();
};


#endif 

