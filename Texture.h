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
		GLuint ID;
		Texture(const char* filename, GLenum location);
		void texLoc(Shader& shader, const char* uniform, GLuint unit);

		void Bind();
		void Unbind();
		void Delete();
};

#endif 
