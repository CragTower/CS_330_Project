#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <stb_image.h>
#include <cstdlib>
#include <iostream>
#include "Shader.h"

class Texture
{
	public:
		// Unique ID for each instance
		GLuint ID;
		// Constructor
		Texture(const char* filename, GLenum location);

		// Sends texture data to GPU
		void texLoc(Shader& shader, const char* uniform, GLuint unit);
		// Binds texture to make it current
		void Bind();
		// Unbinds texture so it can not be modified further
		void Unbind();
		// Deletes texture
		void Delete();
};

#endif 
