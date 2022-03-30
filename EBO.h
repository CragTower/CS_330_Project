#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

class EBO
{
public:
	// Unique ID for each instance
	GLuint ID;
	// Constructor
	EBO(std::vector<GLuint>& indices);

	// Binds EBO
	void Bind();
	// Unbinds EBO
	void Unbind();
	// Deletes EBO
	void Delete();
};


#endif