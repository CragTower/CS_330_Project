#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cerrno>

std::string getFileInfo(const char* filename);

class Shader
{
	public:
		GLuint ID;
		// Constructor
		Shader(const char* vertexShaderSource, const char* fragmentShaderSource);

		// Activate shader program
		void Activate();
		// Deletes shader program
		void Delete();
};

#endif
