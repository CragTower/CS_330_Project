#include "Shader.h"

Shader::Shader(const char* vertexShaderSource, const char* fragmentShaderSource)
{
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];
    
    // Create the shader program
    ID = glCreateProgram();

    // Create the vertex shader object
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderID, 1, &vertexShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderID);
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

	// Create the fragment shader object
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, &fragmentShaderSource, NULL);

    // Compile the fragment shader, and print compilation errors (if any)
    glCompileShader(fragmentShaderID);
    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderID, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Attached compiled shaders to the shader program
    glAttachShader(ID, vertexShaderID);
    glAttachShader(ID, fragmentShaderID);
    
    // Links the shader program and checks for compile errors
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    

    // Delete both Vertex and Fragment shaders
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
}

void Shader::Activate()
{
    // Uses the shader program
    glUseProgram(ID);
}

void Shader::Delete()
{
    // Deletes shader program
    glDeleteProgram(ID);
}



