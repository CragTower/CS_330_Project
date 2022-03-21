/*
**	Justin Holmes
**	Computer Graphics CS-330
**	The purpose of this program is to be able to draw a pyramid to the
**	screen using OpenGL.
*/

#include <iostream>         // cout, cerr
#include <vector>
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

using namespace std; // Uses the standard namespace

// Shader program Macro
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

namespace
{	
	// Window name
	const char* const WIN_NAME = "CS_330_Project";

	// Window width and height
	const int WIN_WIDTH = 800;
	const int WIN_HEIGHT = 600;

	// Stores the GL data relative to a given mesh
	struct GLMesh
	{
		GLuint vao;			// Handle for the vertex array object
		GLuint vbos[2];		// Handle for the vertex buffer objects
		GLuint nIndices;	// Number of indices of the mesh
	};

	// Main window
	// ___________
	// GLFW window variable
	GLFWwindow* window = nullptr;

	// Shader program
	GLuint gProgramId;
}

// User defined functions
// ______________________
bool glInitialize(int, char* [], GLFWwindow** window); // Initialize window
void glResizeWindow(GLFWwindow* window, int width, int height); // Adjusts viewport when window resized
void processInput(GLFWwindow* window);
void drawCylinder(GLfloat radius, GLfloat height, int numSlices, GLubyte R, GLubyte G, GLubyte B);
void glDataSend(GLMesh& mesh, GLfloat verts[], GLushort indices[], int vertsSize, int indicesSize);
void glDestroyMesh(GLMesh& mesh);
bool glCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void glDestroyShaderProgram(GLuint programId);
void glRenderObj1();

/* Vertex Shader Source Code*/
const GLchar* vertexShaderSource = GLSL(440,
	layout(location = 0) in vec3 position; // Vertex data from Vertex Attrib Pointer 0
	layout(location = 1) in vec4 color;  // Color data from Vertex Attrib Pointer 1

	out vec4 vertexColor; // variable to transfer color data to the fragment shader

	//Global variables for the  transform matrices
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	void main()
	{
		gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates
		vertexColor = color; // references incoming color data
	}
	);


/* Fragment Shader Source Code*/
const GLchar* fragmentShaderSource = GLSL(440,
	in vec4 vertexColor; // Variable to hold incoming color data from vertex shader

	out vec4 fragmentColor;

	void main()
	{
		fragmentColor = vec4(vertexColor);
	}
	);



// Main entry point to OpenGL Program
// __________________________________
int main(int argc, char* argv[])
{
	if (!glInitialize(argc, argv, &window))
		return EXIT_FAILURE;

	// Create the object mesh
	//glPyramidMesh(gMesh1);
	//glCubeMesh(gMesh2);
	drawCylinder(0.05, 1.2, 8, 1.0f, 0.0f, 0.0f);

	// Create shader program
	if (!glCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
		return EXIT_FAILURE;

	// Set initial background color to window
	glClearColor(0.0f, 0.0f, 0.0, 1.0f);

	// Render loop
	//____________
	while (!glfwWindowShouldClose(window))
	{
		// User input
		processInput(window);

		// Render this frame
		glRenderObj1();

		// Monitors Input/Output events
		glfwPollEvents();
	}

	// Release data stored in mesh
	glDestroyMesh();

	// Release shader program
	glDestroyShaderProgram(gProgramId);

	// Terminates program successfully
	exit(EXIT_SUCCESS);
}



bool glInitialize(int argc, char* argv[], GLFWwindow** window)
{
	// GLFW: initialize and configure
	// ______________________________
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	GLFWwindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// GLFW: create window
	// ___________________
	* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_NAME, NULL, NULL);
	if (*window == NULL)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(*window);
	glfwSetFramebufferSizeCallback(*window, glResizeWindow);

	// GLEW: initialize
	// ________________
	glewExperimental = GL_TRUE; // Note: if using GLEW version 1.13 or earlier
	GLenum GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult)
	{
		cerr << glewGetErrorString(GlewInitResult) << endl;
		return false;
	}

	// Displays GPU OpenGL version
	cout << "INFO: OpenGL version: " << glGetString(GL_VERSION) << endl;

	return true;
}



void drawCylinder(GLfloat radius, GLfloat height, int numSlices, GLubyte R, GLubyte G, GLubyte B)
{
	// Pre-calculate sines / cosines for given number of slices
	const auto sliceAngleStep = 2.0f * glm::pi<float>() / static_cast<float>(numSlices);
	auto currentSliceAngle = 0.0f;
	std::vector<float> sines, cosines;
	for (auto i = 0; i <= numSlices; i++)
	{
		sines.push_back(sin(currentSliceAngle));
		cosines.push_back(cos(currentSliceAngle));

		// Update slice angle
		currentSliceAngle += sliceAngleStep;
	}

	// Pre-calculate X and Z coordinates
	std::vector<float> x;
	std::vector<float> z;
	for (auto i = 0; i <= numSlices; i++)
	{
		x.push_back(cosines[i] * radius);
		z.push_back(sines[i] * radius);
	}
	
	drawCylWalls(x, z, numSlices, height, R, G, B);

	// Add top cylinder cover
	

}



void drawCylWalls(vector<float> x, vector<float> z, int numSlices, GLfloat height,
				  GLubyte R, GLubyte G, GLubyte B)
{
	GLMesh cMesh;  // Creates new mesh

	// Add cylinder side vertices
	std::vector<float> cylSideVerts;
	for (auto i = 0; i <= numSlices; i++)
	{
		cylSideVerts.push_back(x[i]);
		cylSideVerts.push_back(height / 2.0f);
		cylSideVerts.push_back(z[i]);

		cylSideVerts.push_back(x[i]);
		cylSideVerts.push_back(-height / 2.0f);
		cylSideVerts.push_back(z[i]);
	}
	
	std::vector<short> cylSideIndices;
	for (auto i = 0; i <= numSlices; i++)
	{
		cylSideIndices.push_back(i);
		cylSideIndices.push_back(i + 1);
		cylSideIndices.push_back(i + 2);
	}

	
	
	glDataSend(cMesh, cylSideVerts, cylSideIndices, sizeof(cylSideVerts), sizeof(cylSideIndices));
}



// Packages the data and sends to GPU
void glDataSend(GLMesh& mesh, vector<float> verts, vector<short> indices, int vertsSize, int indicesSize)
{
	glGenVertexArrays(1, &mesh.vao);	// generates vao
	glBindVertexArray(mesh.vao);
	
	// Creates 2 buffers, one for the vertex data, second one for the indices
	glGenBuffers(2, mesh.vbos);		// generates vbos
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, vertsSize * sizeof(float), verts, GL_STATIC_DRAW); // Sends data to GPU

	mesh.nIndices = indicesSize / sizeof(indices[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(float), indices, GL_STATIC_DRAW);

	// Create the Vertex Attribute Pointer
	const GLuint floatsPerVertex = 3;	// Number of coordinates per vertex (x, y, z)
	const GLuint floatsPerColor = 4;	// (R, G, B, A)
	
	//Strides between vertex coords is 6 (x, y, r, g, b, a)
	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor);

	// Instructs the GPU how to handle the vertex buffer obj data
	// __________________________________________________________
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0); // Specifies the initial position of the coords in the buffer

	glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);
}



// Destroys vao and vbos
void glDestroyMesh(GLMesh& mesh)
{
	glDeleteVertexArrays(1, &mesh.vao);
	glDeleteBuffers(2, mesh.vbos);
}



// Renders vertex data to screen
void glRenderObj1()
{
	// Enable z-depth
	glEnable(GL_DEPTH_TEST);

	// Clear the background
	glClearColor(0.0f, 0.2f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Model Matrix
	// ____________
	// Scales shape down or up
	glm::mat4 scale = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));

	// Rotates shape by 55 degrees clockwise on y-axis
	glm::mat4 rotation = glm::rotate(glm::radians(-55.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// Translates object in local space (moves object along the axes)
	glm::mat4 translation = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));

	// Transformations are applied (WARNING: will be applied from right to left)
	glm::mat4 model = translation * rotation * scale;
	// ______________

	// View Matrix
	// ___________
	// Moves camera back by 3 units
	glm::mat4 view = glm::translate(glm::vec3(0.0f, 0.0f, -3.0f));
	// ___________

	// Projection Matrix
	// _________________
	// Create perspective projection (45 degree field of view)
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)WIN_WIDTH / (GLfloat)WIN_HEIGHT, 1.0f, 150.0f);
	// _________________

	// Set the shader to be used
	glUseProgram(gProgramId);
	
	// Receives and sends transformation info to shader program
	GLint modelLoc = glGetUniformLocation(gProgramId, "model");
	GLint viewLoc = glGetUniformLocation(gProgramId, "view");
	GLint projLoc = glGetUniformLocation(gProgramId, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// Activate the VBOs within the VAO
	glBindVertexArray();

	// Draw the object
	glDrawElements(GL_TRIANGLES, .nIndices, GL_UNSIGNED_SHORT, NULL);

	// Deactivate the VAO
	glBindVertexArray(0);

	//Switches back buffer with front buffer
	glfwSwapBuffers(window);
}



// Creates shader program
// ______________________
bool glCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
	// Compilation and linkage error reporting
	int success = 0;
	char infoLog[512];

	// Creates shader program object
	programId = glCreateProgram();

	// Creates vertex and fragment shader objects
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	// Retrieve shader source
	glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
	glShaderSource(fragShaderId, 1, &fragShaderSource, NULL);

	// Compile vertex shader
	glCompileShader(vertexShaderId);
	// Check for shader compile errors and print errors
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;

		return false;
	}

	// Compile fragment shader
	glCompileShader(fragShaderId);
	// Check for shader compile errors and print errors
	glGetShaderiv(fragShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragShaderId, 512, NULL, infoLog);
		cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;

		return false;
	}

	// Attach compiled shaders to shader program
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragShaderId);

	// Links the shader program
	glLinkProgram(programId);
	// Check for linking errors
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
		cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;

		return false;
	}

	// Use shader program
	glUseProgram(programId);

	return true;
}



// Destroys shader program
void glDestroyShaderProgram(GLuint programId)
{
	glDeleteProgram(programId);
}



// Retrieves user input
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}



// Adjusts viewport if and when window is resized
void glResizeWindow(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}