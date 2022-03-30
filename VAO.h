#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include "VBO.h"


class VAO
{
	public:
		// Unique ID every instance
		GLuint ID;
		// Constructor
		VAO();

		// Links vertex attrib pointer
		void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
		// Binds VAO
		void Bind();
		// Unbinds VAO
		void Unbind();
		// Deletes VAO
		void Delete();
};


#endif

