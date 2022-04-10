#include "Shader.h"

std::string getFileInfo(const char* filename)
{
    // Reads in data from file
    std::ifstream in(filename, std::ios::binary);
    if (in)
    {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }
    throw(errno);
}

Shader::Shader(const char* vertexShaderFile, const char* fragmentShaderFile)
{
    // Assigns string data from files to local variables
    std::string vertexGLSLCode = getFileInfo(vertexShaderFile);
    std::string fragmentGLSLCode = getFileInfo(fragmentShaderFile);

    // Converts string data from files to const char arrays
    const char* vertexSource = vertexGLSLCode.c_str();
    const char* fragmentSource = fragmentGLSLCode.c_str();
    
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];
    
    // Create the shader program
    ID = glCreateProgram();

    // Create the vertex shader object
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderID, 1, &vertexSource, NULL);

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
	glShaderSource(fragmentShaderID, 1, &fragmentSource, NULL);

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

    // Delete both Vertex and Fragment shader variables now they are no longer being used
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



