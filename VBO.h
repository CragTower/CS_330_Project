#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

// Creation of new struct to hold vertex attribs
struct Vertex
{
	glm::vec3 position;
	glm::vec4 color;
};


class VBO
{
public:
	// Unique ID for each instance
	GLuint ID;
	// Constructor
	VBO(std::vector<Vertex>& verts);

	// Binds VBO
	void Bind();
	// Unbinds VBO
	void Unbind();
	// Deletes VBO
	void Delete();
};


#endif