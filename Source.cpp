/*
**	Justin Holmes
**	Computer Graphics CS-330
**	The purpose of this program is to draw and render a 3D environment
**	This update includes a cylinder with a pyramid to resemble a pen
*/

#include <iostream>         
#include <cstdlib>          
#include <vector>
#include <GL/glew.h>        
#include <GLFW/glfw3.h>     

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std; // Standard namespace

/* Shader program Macro */
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "CS_330_Project_Milestone_4.5"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // Creates a structure to hold the Mesh data
    struct GLMesh
    {
        GLuint vao;         // Handle for the vertex array object
        GLuint vbos[2];     // Handles for the vertex buffer objects
        GLuint nIndices;    // Number of indices of the mesh
    };

    // Global variable to hold main GLFW window
    GLFWwindow* gWindow = nullptr;
    // Global variable to hold the different instances of Mesh data
    GLMesh gMesh1;
    GLMesh gMesh2;
    GLMesh gMesh3;
    // Global variable to hold Shader program
    GLuint gProgramId;

    // Camera position variables
    glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    bool firstMouse = true;     // Value to reference if this is the first mouse event (program start up)
    float yaw = -90.0f;         // Set to -90 to offset direction vector
    float pitch = 0.0f;
    float lastX = WINDOW_WIDTH / 2.0;
    float lastY = WINDOW_HEIGHT / 2.0;
    float fov = 45.0f;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    float speed = 1.0f;

    bool isViewPort = true;

    glm::mat4 projection;// = glm::perspective(glm::radians(fov), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
}


// User defined functions
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void generateVAO_VBOS(GLMesh& mesh);
void UProcessInput(GLFWwindow* window, int key, int scancode, int action, int mods);
void drawCylinder(GLMesh& mesh, GLfloat height, GLfloat radius, int numSlices);
void drawPyramid(GLMesh& mesh);
void drawPlane(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);
void URender(GLMesh& mesh, GLfloat _scale, GLfloat rotX, GLfloat rotY, GLfloat rotZ, GLfloat translX, GLfloat translY, GLfloat translZ);
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


// Vertex Shader Code (GLSL)
// _________________________
const GLchar* vertexShaderSource = GLSL(440,
    layout(location = 0) in vec3 position; // Vertex data from Vertex Attrib Pointer 0
layout(location = 1) in vec4 color;  // Color data from Vertex Attrib Pointer 1

out vec4 vertexColor; // variable to transfer color data to the fragment shader

//Global variables for the transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates
    vertexColor = color; // references incoming color data
}
);


// Fragment Shader Code (GLSL)
// ___________________________
const GLchar* fragmentShaderSource = GLSL(440,
    in vec4 vertexColor; // Variable to hold incoming color data from vertex shader

out vec4 fragmentColor;

void main()
{
    if (length(vertexColor) > 0)
        fragmentColor = vec4(vertexColor);
    else
        fragmentColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);
}
);


// Main entry into program
// _______________________
int main(int argc, char* argv[])
{
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Create the shader program
    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
        return EXIT_FAILURE;

    // Generates the VAO's and VBO's for each Mesh created
    generateVAO_VBOS(gMesh1);
    generateVAO_VBOS(gMesh2);
    generateVAO_VBOS(gMesh3);

    // render loop
    // ___________
    while (!glfwWindowShouldClose(gWindow))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Enable z-depth to render closest Z coords
        glEnable(GL_DEPTH_TEST);

        // Clear the frame and z buffers
        glClearColor(0.0f, 0.3f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Sends data to GPU, draws cylinder on back buffer, and manages obj matrices
        drawCylinder(gMesh1, 1.2f, 0.1f, 100);
        URender(gMesh1, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);

        // Sends data to GPU, draws pyramid on back buffer, and manages obj matrices
        drawPyramid(gMesh2);
        URender(gMesh2, 0.19f, 0.0f, 1.0f, 0.0f, 0.0f, 0.6f, 0.0f);

        // Draws plane
        drawPlane(gMesh3);
        URender(gMesh3, 4.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);

        // Swaps front buffer with back buffer 
        glfwSwapBuffers(gWindow);

        // Polls events (monitors Input/Output events such as keystrokes or mouse movements)
        glfwPollEvents();
    }

    // Release mesh data
    UDestroyMesh(gMesh1);
    UDestroyMesh(gMesh2);

    // Release shader program
    UDestroyShaderProgram(gProgramId);

    // Terminates the program successfully
    exit(EXIT_SUCCESS);
}


// Initialize GLFW, GLEW, and create a window
// __________________________________________
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);                  // Highest version of opengl allowed
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);                  // Lowest version of opengl allowed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // Tells which profile of opengl we will use (modern functions)

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    // Fail check for window
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    // Makes the window current
    glfwMakeContextCurrent(*window);
    // Handles the frame buffer during window resize
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);
    // Handles the mouse movement events
    glfwSetCursorPosCallback(*window, mouse_callback);
    // Handles the scroll movement events
    glfwSetScrollCallback(*window, scroll_callback);
    // Handles the keyboard events
    glfwSetKeyCallback(*window, UProcessInput);

    // Tells GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLEW: initialize
    // Note: if using GLEW version 1.13 or earlier
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    // Fail check for GLEW init
    if (GLEW_OK != GlewInitResult)
    {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}



// Processes input from user; monitors keystrokes and mouse movement and processes accordingly
// ___________________________________________________________________________________________
void UProcessInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)      // If ESCAPE key is pressed close program
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        if (isViewPort)
            isViewPort = false;
        else
            isViewPort = true;
    }
}



// When window is resized the viewport is also changed
// ___________________________________________________
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}



// Functioned called to render a frame
// ___________________________________
void URender(GLMesh& mesh, GLfloat _scale, GLfloat rotX, GLfloat rotY, GLfloat rotZ, GLfloat translX, GLfloat translY, GLfloat translZ)
{

    // Model matrix
    // Creates matrix for scaling object
    glm::mat4 scale = glm::scale(glm::vec3(_scale, _scale, _scale));
    // Creates matrix for rotating object
    glm::mat4 rotation = glm::rotate(glm::radians(90.0f), glm::vec3(rotX, rotY, rotZ));
    // Creates matrix for moving object (x, y, or z location)
    glm::mat4 translation = glm::translate(glm::vec3(translX, translY, translZ));
    // Creates the model matrix by applying the object manipulations
    glm::mat4 model = translation * rotation * scale;

    // View matrix
    // Moves the "camera" (x, y, or z)
    glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    float radius = 3.0f;
    float camX = static_cast<float>(sin(glfwGetTime()) * radius);
    float camZ = static_cast<float>(cos(glfwGetTime()) * radius);
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    //glm::mat4 view = glm::translate(glm::vec3(0.0f, 0.0f, -2.0f));

    // Projection matrix
    // Creates a perspective projection (gives depth to the 2D screen locations)

    if (isViewPort)
        projection = glm::perspective(glm::radians(fov), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    else
        projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);



    // Set the shader to be used
    glUseProgram(gProgramId);

    // Retrives variable names from current shader program and assigns them to local variables
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");

    // Sends matrix manipulation information back to GPU
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Makes mesh in argument the current mesh
    glBindVertexArray(mesh.vao);
    // Draws the current mesh data onto the back buffer
    glDrawElements(GL_TRIANGLES, mesh.nIndices, GL_UNSIGNED_INT, NULL);
    // Unbinds the current mesh so as to not alter it
    glBindVertexArray(0);
}



// Draws Plane
void drawPlane(GLMesh& mesh)
{
    vector<GLfloat> verts =
    {
        // Vertices
        -0.5f, 0.0f, -0.5f,        1.0f, 1.0f, 1.0f, 1.0f,      // Back left
        -0.5f, 0.0f,  0.5f,        1.0f, 1.0f, 1.0f, 1.0f,      // Front left
         0.5f, 0.0f, -0.5f,        1.0f, 1.0f, 1.0f, 1.0f,      // Back right
         0.5f, 0.0f,  0.5f,        1.0f, 1.0f, 1.0f, 1.0f       // Front right
    };

    vector<int> indices =
    {
        0, 1, 2,
        1, 2, 3
    };

    // Position range of x, y, z coords in vector
    const GLuint floatsPerVertex = 3;
    // Position range of color coords in vector
    const GLuint floatsPerColor = 4;

    // Binds the mesh to make it current
    glBindVertexArray(mesh.vao);

    // Binds the buffer to make it current
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]);
    // Sends vertex vector data to GPU
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW);

    // Calculates index size for later use
    mesh.nIndices = indices.size();
    // Binds the buffer to make it current
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    // Sends index vector data to GPU
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    // Number of floats between each vertex point (how many bytes between vertex locations in vector
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor);

    // Create Vertex Attribute Pointers for use in shader program
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    // Enables vertex attrib array starting at index 0
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    // Unbinds vertex array so as to not modify it anymore
    glBindVertexArray(0);

}


/*
    -------------------------------------
    Calculates vertices for cylinder
    Maps the indices for vertex triangles
    Sends data to GPU for later use
    -------------------------------------
*/
void drawCylinder(GLMesh& mesh, GLfloat height, GLfloat radius, int numSlices)
{
    // Creates vector for calculated points for a 2D circle
    std::vector<GLfloat> circleArray;
    // Creates vector for vertex storage of whole cylinder
    std::vector<GLfloat> vertexArray;

    // Determines how many slices around a circle
    float sectorStep = 2.0f * glm::pi<float>() / numSlices;
    float sectorAngle;

    for (int i = 0; i <= numSlices; i++)
    {
        sectorAngle = i * sectorStep;               // Determines angle
        circleArray.push_back(cos(sectorAngle));    // x
        circleArray.push_back(0);                   // y
        circleArray.push_back(sin(sectorAngle));    // z
    }

    // Creates top and bottom vertices for side of cylinder
    for (int i = 0; i < 2; ++i)
    {
        float h = (-height / 2.0f) + (i * height);  // Determines height

        for (int j = 0, k = 0; j <= numSlices; j++, k += 3)
        {
            float ux = circleArray[k];          // x location holder
            float uy = circleArray[k + 1];      // y location holder
            float uz = circleArray[k + 2];      // z location holder
            // position vector
            vertexArray.push_back(ux * radius);
            vertexArray.push_back(h);
            vertexArray.push_back(uz * radius);
        }
    }

    // Determinces the center point for bottom and top circles for cylinder
    // Will be used for index vertex creation
    int baseCenterIndex = (int)vertexArray.size() / 3;
    int topCenterIndex = baseCenterIndex + numSlices + 1;

    // Creates top and bottom vertices for the "lids" of cylinder
    for (int i = 0; i < 2; i++)
    {
        float h = -height / 2.0f + i * height;      // determines height

        // Center point
        vertexArray.push_back(0);
        vertexArray.push_back(h);
        vertexArray.push_back(0);

        for (int j = 0, k = 0; j < numSlices; ++j, k += 3)
        {
            float ux = circleArray[k];      // x location holder
            float uz = circleArray[k + 2];  // z location holder
            // Position vector
            vertexArray.push_back(ux * radius);
            vertexArray.push_back(h);
            vertexArray.push_back(uz * radius);
        }
    }

    // Generate vector for indices mapping
    std::vector<int> indices;
    int k1 = 0;                       // 1st vertex index at base
    int k2 = numSlices + 1;           // 1st vertex index at top

    // Indices for the side surface
    for (int i = 0; i < numSlices; ++i, ++k1, ++k2)
    {
        // 2 triangles per sector
        // k1 => k1+1 => k2
        indices.push_back(k1);
        indices.push_back(k1 + 1);
        indices.push_back(k2);

        // k2 => k1+1 => k2+1
        indices.push_back(k2);
        indices.push_back(k1 + 1);
        indices.push_back(k2 + 1);
    }

    // Indices for the base surface
    for (int i = 0, k = baseCenterIndex + 1; i < numSlices; ++i, ++k)
    {
        if (i < numSlices - 1)
        {
            indices.push_back(baseCenterIndex);
            indices.push_back(k + 1);
            indices.push_back(k);
        }
        else // last triangle
        {
            indices.push_back(baseCenterIndex);
            indices.push_back(baseCenterIndex + 1);
            indices.push_back(k);
        }
    }

    // Indices for the top surface
    for (int i = 0, k = topCenterIndex + 1; i < numSlices; ++i, ++k)
    {
        if (i < numSlices - 1)
        {
            indices.push_back(topCenterIndex);
            indices.push_back(k);
            indices.push_back(k + 1);
        }
        else // last triangle
        {
            indices.push_back(topCenterIndex);
            indices.push_back(k);
            indices.push_back(topCenterIndex + 1);
        }
    }

    // Position range of x, y, z coords in vector
    const GLuint floatsPerVertex = 3;
    // Position range of color coords in vector
    //const GLuint floatsPerColor = 4;

    // Binds the mesh to make it current
    glBindVertexArray(mesh.vao);

    // Binds the buffer to make it current
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]);
    // Sends vertex vector data to GPU
    glBufferData(GL_ARRAY_BUFFER, vertexArray.size() * sizeof(GLfloat), vertexArray.data(), GL_STATIC_DRAW);

    // Calculates index size for later use
    mesh.nIndices = indices.size();
    // Binds the buffer to make it current
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    // Sends index vector data to GPU
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    // Number of floats between each vertex point (how many bytes between vertex locations in vector
    GLint stride = sizeof(float) * floatsPerVertex;

    // Create Vertex Attribute Pointers for use in shader program
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    // Enables vertex attrib array starting at index 0
    glEnableVertexAttribArray(0);

    // Unbinds vertex array so as to not modify it anymore
    glBindVertexArray(0);
}



/*
    -------------------------------------
    Calculates vertices for pyramid
    Maps the indices for vertex triangles
    Sends data to GPU for later use
    -------------------------------------
*/
void drawPyramid(GLMesh& mesh)
{
    // Vertex coords
    vector<GLfloat> vertices =
    {
        // Vertices			        // Colors (r, g, b)
    -0.5f,  0.0f,  0.5f,        1.0f, 0.0f, 0.0f, 1.0f, 	// Lower left corner
    -0.5f,  0.0f, -0.5f,        0.0f, 1.0f, 0.0f, 1.0f, 	// Upper left corner
     0.5f,	0.0f, -0.5f,        0.0f, 0.0f, 1.0f, 1.0f,	    // Upper right corner
     0.5f,  0.0f,  0.5f,        0.3f, 0.8f, 0.5f, 1.0f, 	// Lower right corner
     0.0f,  0.8f,  0.0f,        0.3f, 0.8f, 0.5f, 1.0f	    // Lower right corner
    };

    // Indices combinations that make up shape
    vector<GLuint> indices =
    {
        0, 1, 2,
        0, 2, 3,
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
    };

    // Position range of x, y, z coords in vector
    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;

    // Binds the mesh to make it current
    glBindVertexArray(mesh.vao);

    // Binds the 1st buffer of mesh to make it current
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]);
    // Sends the vertex data to GPU
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    // Calculates number of indices in vector for later use
    mesh.nIndices = indices.size();
    // Binds the 2nd buffer of mesh to make it current 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    // Sends the index data to GPU
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    // Number of floats between each vertex point (how many bytes between vertex locations in vector)
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor);

    // Create Vertex Attribute Pointers for use in shader program
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    // Enables vertex attrib array starting of position 0
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    // Unbinds the mesh so as to not modify it anymore
    glBindVertexArray(0);
}



// Destryoys Mesh 
// ______________
void UDestroyMesh(GLMesh& mesh)
{
    // Deletes vao position of mesh
    glDeleteVertexArrays(1, &mesh.vao);
    // Deletes vbo position of mesh
    glDeleteBuffers(2, mesh.vbos);
}



// Implements the UCreateShaders function
// ______________________________________
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];

    // Create a Shader program object.
    programId = glCreateProgram();

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrieve the shader source code and assigns it to variables in first argument slot
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId);
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Compile the fragment shader, and print compilation errors (if any)
    glCompileShader(fragmentShaderId);
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Attached compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    // Links the shader program and checks for compile errors
    glLinkProgram(programId);
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Uses the shader program
    glUseProgram(programId);

    return true;
}



// Destroys the shader program
// ___________________________
void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}



// Generates the VAO's and VBO's for given mesh
// ____________________________________________
void generateVAO_VBOS(GLMesh& mesh)
{
    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glGenBuffers(2, mesh.vbos);
}



// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}



void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    cout << "Mouse Scroll " << yoffset << endl;
    speed -= yoffset / 10;
    if (speed > 10.0f)
        speed = 10.0f;
    if (speed < 1.0f)
        speed = 1.0f;
}