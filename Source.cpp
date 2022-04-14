/*
**	Justin Holmes
**	Computer Graphics CS-330
**	The purpose of this program is to draw and render a 3D environment.
**	This update includes the introduction of a new light source.
*/

#include "Mesh.h"
#include "Texture.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std; // Standard namespace

// Unnamed namespace
namespace
{
    // Variable for window title
    const char* const WINDOW_TITLE = "CS_330_Project_Milestone_6.5"; 

    // Variables for window width and height
    const int WINDOW_WIDTH = 1024;
    const int WINDOW_HEIGHT = 1024;

    // Global variable to hold main GLFW window
    GLFWwindow* gWindow = nullptr;

    // FIXME: Create Camera class
    // Camera position variables
    glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    bool firstMouse = true;             // Value to reference if this is the first mouse event (program start up)
    float yaw = -90.0f;                 // Set to -90 to offset direction vector
    float pitch = 0.0f;
    float lastX = WINDOW_WIDTH / 2.0;   // Gets center point of window width
    float lastY = WINDOW_HEIGHT / 2.0;  // Gets center point of window height
    float fov = 45.0f;                  // Field of View

    float deltaTime = 0.0f;             // Time difference between current and last update
    float lastFrame = 0.0f;             // Holds the time of last update
    float speed = 5.0f;                 // Current speed for camer movement

    bool isViewPort = true;             // Is the current view "perspective"

    glm::mat4 projection;               // Variable to hold projection matrix

    // Light color
    glm::vec3 gLightColor1(1.0f, 1.0f, 1.0f);
    //glm::vec3 gLightColor2(1.0f, 1.0f, 1.0f);

    // Light position
    glm::vec3 gLightPositionKey(1.5f, 3.0f, 3.0f);
    //glm::vec3 gLightPositionFill(-1.5f, 0.5f, -1.0f);
    glm::vec3 gLightScale(0.1f);

    bool gIsLampOrbiting = true;
}

// User defined functions
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void toggleInput(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void URenderLight(Mesh& Mesh, Shader newLightShader);
void URender(Mesh& Mesh, Shader newShader, GLfloat _scale, GLfloat deg, GLfloat rotX, GLfloat rotY, GLfloat rotZ, GLfloat translX, GLfloat translY, GLfloat translZ);
static Mesh drawCylinder(GLfloat height, GLfloat radius, int numSlices);
static Mesh drawPyramid();
static Mesh drawPlane();
static Mesh drawCube();

// Main entry into program
// _______________________
int main(int argc, char* argv[])
{
    // Initialize the window
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Creates new shader program
    Shader newShader("VertexShader.txt", "FragmentShader.txt");
    // Creates new light shader program
    Shader newLightShader("LightSourceVertexShader.txt", "LightSourceFragShader.txt");

    // Sends shape information to GPU
    Mesh cylCan     = drawCylinder(1.0f, 0.3f, 100);
    Mesh cylCanLid  = drawCylinder(0.001f, 0.3f, 100);
    Mesh cylinder   = drawCylinder(1.0f, 0.05f, 100);
    Mesh pyramid    = drawPyramid();
    Mesh floor      = drawPlane();
    Mesh box        = drawCube();

    // Creates and stores textures
    Texture sodaCanBody("Dr_Pepper_Can.jpg", GL_TEXTURE0);
    Texture sodaCanTop("soda_can_top.jpg", GL_TEXTURE0);
    Texture penHead("PenHead1.jpg", GL_TEXTURE0);
    Texture penBody("PenBody1.jpg", GL_TEXTURE0);
    Texture planeFloor("brickwall.jpg", GL_TEXTURE0);
    Texture crackerBox("whole_box.jpg", GL_TEXTURE0);
    
    sodaCanBody.texLoc(newShader, "tex0", 0);
    sodaCanTop.texLoc(newShader, "tex0", 0);
    penHead.texLoc(newShader, "tex0", 0);
    penBody.texLoc(newShader, "tex0", 0);
    planeFloor.texLoc(newShader, "tex0", 0);
    crackerBox.texLoc(newShader, "tex0", 0);
    
    // render loop
    // ___________
    while (!glfwWindowShouldClose(gWindow))
    {
        // Gets delta time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process user inputs
        processInput(gWindow);

        // Enable z-depth to render closest Z coords
        glEnable(GL_DEPTH_TEST);

        // Clear the frame and z buffers
        glClearColor(0.0f, 0.3f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        URender(box, newShader, 1.2f, -40.0f, 0.0f, 1.0f, 0.0f, -0.5f, 0.0f, -0.1f);
        box.Draw(newShader, crackerBox);

        URender(cylCan, newShader, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.2f, 0.0f, 0.0f);
        cylCan.Draw(newShader, sodaCanBody);

        URender(cylCanLid, newShader, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.2f, 1.0f, 0.0f);
        cylCanLid.Draw(newShader, sodaCanTop);

        // Draws cylinder on back buffer, and manages obj matrices
        URender(cylinder, newShader, 1.0f, -14.5f, 1.0f, 0.0f, 0.0f, 0.2f, 0.0f, 0.6f);
        cylinder.Draw(newShader, penBody);
        
        // Draws pyramid on back buffer, and manages obj matrices
        URender(pyramid, newShader, 0.1f, -14.5f, 1.0f, 0.0f, 0.0f, 0.2f, 0.97f, 0.35f);
        pyramid.Draw(newShader, penHead);

        // Draws plane on back buffer, and manages obj matrices
        URender(floor, newShader, 6.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        floor.Draw(newShader, planeFloor);

        // Draws the light source in the shape of a plane
        URenderLight(pyramid, newLightShader);
        pyramid.Draw(newLightShader, penHead);
        
        // Swaps front buffer with back buffer 
        glfwSwapBuffers(gWindow);

        // Polls events (monitors Input/Output events such as keystrokes or mouse movements)
        glfwPollEvents();
    }
    
    // Delete's shader programs
    newShader.Delete();
    newLightShader.Delete();
    // Delete's textures
    penHead.Delete();
    penBody.Delete();
    planeFloor.Delete();

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
    glfwSetKeyCallback(*window, toggleInput);
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
void processInput(GLFWwindow* window)
{
    // Closes program
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = speed * deltaTime;
    // Move forward
    if (glfwGetKey(window, GLFW_KEY_W) || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    // Move camera backward
    if (glfwGetKey(window, GLFW_KEY_S) || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    // Move camera left
    if (glfwGetKey(window, GLFW_KEY_A) || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    // Move camera right
    if (glfwGetKey(window, GLFW_KEY_D) || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    // Move camera up
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    // Move camera down
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;
}



// Switches between perspective and orthogonal view
// ________________________________________________
void toggleInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Swaps between perspective view and orthogonal view
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
void URender(Mesh& Mesh, Shader newShader, GLfloat _scale, GLfloat deg, GLfloat rotX, GLfloat rotY, GLfloat rotZ, GLfloat translX, GLfloat translY, GLfloat translZ)
{
    const float angularVelocity = glm::radians(5.0f);
    if (gIsLampOrbiting)
    {
        glm::vec4 newPosition = glm::rotate(angularVelocity * deltaTime, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(gLightPositionKey, 1.0f);
        gLightPositionKey.x = newPosition.x;
        gLightPositionKey.y = newPosition.y;
        gLightPositionKey.z = newPosition.z;
    }
    
    
    
    // Model matrix
    // ------------
    // Creates matrix for scaling object
    glm::mat4 scale = glm::scale(glm::vec3(_scale, _scale, _scale));
    // Creates matrix for rotating object
    glm::mat4 rotation = glm::rotate(glm::radians(deg), glm::vec3(rotX, rotY, rotZ));
    // Creates matrix for moving object (x, y, or z location)
    glm::mat4 translation = glm::translate(glm::vec3(translX, translY, translZ));
    // Creates the model matrix by applying the object manipulations
    glm::mat4 model = translation * rotation * scale;

    // View matrix
    // -----------
    // Moves the "camera" (x, y, or z)
    glm::mat4 view = glm::mat4(1.0f); // Initializes to identity matrix
    //float radius = 3.0f;
    //float camX = static_cast<float>(sin(glfwGetTime()) * radius);
    //float camZ = static_cast<float>(cos(glfwGetTime()) * radius);
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    // Projection matrix
    // -----------------
    // Swaps between perspective view and orthogonal view
    if (isViewPort)
        projection = glm::perspective(glm::radians(fov), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    else
        projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 100.0f);

    // Set the shader to be used
    newShader.Activate();

    // Retrives variable names from current shader program and assigns them to local variables
    GLint modelLoc = glGetUniformLocation(newShader.ID, "model");
    GLint viewLoc = glGetUniformLocation(newShader.ID, "view");
    GLint projLoc = glGetUniformLocation(newShader.ID, "projection");

    // Sends matrix manipulation information back to GPU
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Cube Shader program for the cub color, light color, light position, and camera position
    GLint lightColorLoc = glGetUniformLocation(newShader.ID, "lightColor");
    GLint lightColorFillLoc = glGetUniformLocation(newShader.ID, "lightColorFill");
    GLint lightPositionLoc = glGetUniformLocation(newShader.ID, "lightPos");
    GLint lightPositionFillLoc = glGetUniformLocation(newShader.ID, "lightPosFill");
    GLint viewPositionLoc = glGetUniformLocation(newShader.ID, "viewPosition");

    glUniform3f(lightColorLoc, gLightColor1.r, gLightColor1.g, gLightColor1.b);
    //glUniform3f(lightColorFillLoc, gLightColor2.r, gLightColor2.g, gLightColor2.b);
    glUniform3f(lightPositionLoc, gLightPositionKey.x, gLightPositionKey.y, gLightPositionKey.z);
    //glUniform3f(lightPositionFillLoc, gLightPositionFill.x, gLightPositionFill.y, gLightPositionFill.z);
    glUniform3f(viewPositionLoc, cameraPos.x, cameraPos.y, cameraPos.z);
}



void URenderLight(Mesh& Mesh, Shader newLightShader)
{
    glUseProgram(newLightShader.ID);

    //Transform the smaller cube used as a visual que for the light source
    glm::mat4 model = glm::translate(gLightPositionKey) * glm::scale(gLightScale);

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    // Swaps between perspective view and orthogonal view
    if (isViewPort)
        projection = glm::perspective(glm::radians(fov), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    else
        projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 100.0f);

    // Reference matrix uniforms from the Lamp Shader program
    GLint modelLoc = glGetUniformLocation(newLightShader.ID, "model");
    GLint viewLoc = glGetUniformLocation(newLightShader.ID, "view");
    GLint projLoc = glGetUniformLocation(newLightShader.ID, "projection");

    // Pass matrix data to the Lamp Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Deactivate the Vertex Array Object and shader program
    glBindVertexArray(0);
    glUseProgram(0);
}


// FIXME: Refactor code to auto generate plane with length and width from user input
// FIXME: Refactor code into a class
// Draws Plane
static Mesh drawPlane()
{
    // Vector for plane vertices
    std::vector<Vertex> verts =
    {
                // Vertices                          // Normal                    // Texture Coords
        Vertex{glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},      // Back left
        Vertex{glm::vec3(-0.5f, 0.0f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},      // Front left
        Vertex{glm::vec3( 0.5f, 0.0f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},      // Back right
        Vertex{glm::vec3( 0.5f, 0.0f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)}       // Front right
    };

    // Vector of indexes for triangle configuration
    std::vector<GLuint> indices =
    {
        0, 1, 2,
        1, 2, 3
    };

    // Returns a new Mesh
    Mesh floor(verts, indices);
    return floor;
}


// FIXME: Refactor code to auto generate cube with length, width, and height user inputs
// FIXME: Refactor code into a class
// Draws rectangle object
static Mesh drawCube()
{
    std::vector<Vertex> verts =
    {
                    // Position                         // Normals              // Texture               // Bottom face
        Vertex{glm::vec3(-0.2f, 0.0f,  1.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.6f, 0.2f)},     // Bottom Front left corner
        Vertex{glm::vec3( 0.2f, 0.0f,  1.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.5f, 0.2f)},     // Bottom Front right corner
        Vertex{glm::vec3(-0.2f, 0.0f, -1.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.6f, 0.8f)},     // Bottom Back left corner
        Vertex{glm::vec3( 0.2f, 0.0f, -1.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.5f, 0.8f)},     // Bottom Back right corner
                                       
                                                                                                         // Left face
        Vertex{glm::vec3(-0.2f, 0.0f,  1.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.6f, 0.2f)},     // Bottom Front left corner
        Vertex{glm::vec3(-0.2f, 0.0f, -1.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.6f, 0.8f)},     // Bottom Back left corner
        Vertex{glm::vec3(-0.2f, 1.0f, -1.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.8f)},     // Top Back left corner
        Vertex{glm::vec3(-0.2f, 1.0f,  1.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.2f)},     // Top Front left corner
                                       
                                                                                                        // Right face
        Vertex{glm::vec3( 0.2f, 1.0f,  1.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.2f, 0.2f)},     // Top Front right corner
        Vertex{glm::vec3( 0.2f, 1.0f, -1.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.2f, 0.8f)},     // Top Back right corner
        Vertex{glm::vec3( 0.2f, 0.0f,  1.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.5f, 0.2f)},     // Bottom Front right corner
        Vertex{glm::vec3( 0.2f, 0.0f, -1.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.5f, 0.8f)},     // Bottom Back right corner
                                       
                                                                                                        // Front face
        Vertex{glm::vec3(-0.2f, 0.0f,  1.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.5f, 0.0f)},     // Bottom Front left corner
        Vertex{glm::vec3( 0.2f, 0.0f,  1.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.5f, 0.2f)},     // Bottom Front right corner
        Vertex{glm::vec3(-0.2f, 1.0f,  1.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.2f, 0.0f)},     // Top Front left corner
        Vertex{glm::vec3( 0.2f, 1.0f,  1.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.2f, 0.2f)},     // Top Front right corner
                                       
                                                                                                        // Back face
        Vertex{glm::vec3( 0.2f, 0.0f, -1.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.5f, 0.8f)},     // Bottom Back right corner
        Vertex{glm::vec3(-0.2f, 0.0f, -1.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.5f, 1.0f)},     // Bottom Back left corner
        Vertex{glm::vec3( 0.2f, 1.0f, -1.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.2f, 0.8f)},     // Top Back right corner
        Vertex{glm::vec3(-0.2f, 1.0f, -1.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.2f, 1.0f)},     // Top Front left corner
                                       
                                                                                                        // Top face
        Vertex{glm::vec3(-0.2f, 1.0f,  1.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.2f)},     // Top Front left corner
        Vertex{glm::vec3( 0.2f, 1.0f,  1.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.2f, 0.2f)},     // Top Front right corner
        Vertex{glm::vec3(-0.2f, 1.0f, -1.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.8f)},     // Top Back left corner
        Vertex{glm::vec3( 0.2f, 1.0f, -1.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.2f, 0.8f)},     // Top Back right corner
    };

    std::vector<GLuint> indices =
    {
        0, 1, 2,        // Bottom face
        1, 2, 3,

        4, 5, 6,        // Left face
        4, 6, 7,

        8, 9, 10,       // Right face
        9, 10, 11,

        12, 13, 14,     // Front face
        13, 14, 15,

        16, 17, 18,     // Back face
        17, 18, 19,

        20, 21, 22,     // Top face
        21, 22, 23
    };

    Mesh cube(verts, indices);
    return cube;
}


// FIXME: Refactor code to calc a bottom and top circle radius to make cones or cylinders
// FIXME: Refactor code into a class
/*
    -------------------------------------
    Calculates vertices for cylinder
    Maps the indices for vertex triangles
    Sends data to GPU for later use
    -------------------------------------
*/
static Mesh drawCylinder(GLfloat height, GLfloat radius, int numSlices)
{
    // Creates vector for calculated points for a 2D circle
    std::vector<GLfloat> circleArray;
    // Creates vector for vertex storage of whole cylinder
    std::vector<Vertex> vertexArray;

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
        float h = i * height;//(-height / 2.0f) + (i * height);  // Determines height

        for (int j = 0, k = 0; j <= numSlices; j++, k += 3)
        {
            float ux = circleArray[k];          // x location holder
            float uy = circleArray[k + 1];      // y location holder
            float uz = circleArray[k + 2];      // z location holder
            
            vertexArray.push_back(Vertex{ glm::vec3(ux * radius, h, uz * radius), 
                                          glm::vec3(ux, uy, uz),
                                          glm::vec2(j * .01, h) });
        }
    }
    
    // Determinces the center point for bottom and top circles for cylinder
    // Will be used for index vertex creation
    int baseCenterIndex = (int)vertexArray.size();
    int topCenterIndex = baseCenterIndex + numSlices + 1;

    // Creates top and bottom vertices for the "lids" of cylinder
    for (int i = 0; i < 2; i++)
    {
        float h = i * height;    // determines height
        float ny = -1 + i * 2;

        vertexArray.push_back(Vertex{ glm::vec3(0, h, 0), glm::vec3(0, ny, 0), glm::vec2(0.5f, 0.5f)});

        for (int j = 0, k = 0; j < numSlices; ++j, k += 3)
        {
            float ux = circleArray[k];      // x location holder
            float uz = circleArray[k + 2];  // z location holder
            
            vertexArray.push_back(Vertex{ glm::vec3(ux * radius, h, uz * radius), 
                                          glm::vec3(0, ny, 0),
                                          glm::vec2(-ux * 0.5f + 0.5f, -uz * 0.5f + 0.5f)});
            
        }
    }
    
    // Generate vector for indices mapping
    std::vector<GLuint> indices;
    int k1 = 0;                       // 1st vertex index at base
    int k2 = numSlices + 1;           // 1st vertex index at top

    // Indices for the side surface
    for (int i = 0; i < numSlices; ++i, ++k1, ++k2)
    {
        // 2 triangles per sector
        // k1 => k1+1 => k2
        indices.push_back(k1);
        indices.push_back(k2);
        indices.push_back(k1 + 1);

        // k2 => k1+1 => k2+1
        indices.push_back(k2);
        indices.push_back(k2 + 1);
        indices.push_back(k1 + 1);
    }
    
    // Indices for the base surface
    for (int i = 0, k = baseCenterIndex + 1; i < numSlices; ++i, ++k)
    {
        if (i < numSlices - 1)
        {
            indices.push_back(baseCenterIndex);
            indices.push_back(k);
            indices.push_back(k + 1);
        }
        else // last triangle
        {
            indices.push_back(baseCenterIndex);
            indices.push_back(k);
            indices.push_back(baseCenterIndex + 1);
        }
    }

    // Indices for the top surface
    for (int i = 0, k = topCenterIndex + 1; i < numSlices; ++i, ++k)
    {
        if (i < numSlices - 1)
        {
            indices.push_back(topCenterIndex);
            indices.push_back(k + 1);
            indices.push_back(k);
        }
        else // last triangle
        {
            indices.push_back(topCenterIndex);
            indices.push_back(topCenterIndex + 1);
            indices.push_back(k);
        }
    }

    // Returns a new Mesh
    Mesh cyl(vertexArray, indices);
    return cyl;
}


// FIXME: Refactor code to auto generate a pyramid with length, width, height input parameters
// FIXME: Refactor code into class
/*
    -------------------------------------
    Calculates vertices for pyramid
    Maps the indices for vertex triangles
    Sends data to GPU for later use
    -------------------------------------
*/
static Mesh drawPyramid()
{
    // Vertex coords
    vector<Vertex> vertices =
    {
        // Vertices			                      // Normals            // Texture Coords
    Vertex{glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)}, 	// (0)Front left corner
    Vertex{glm::vec3( 0.5f, 0.0f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)}, 	// (1)Front left corner
    Vertex{glm::vec3( 0.5f, 0.0f,  0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
    Vertex{glm::vec3(-0.5f, 0.0f,  0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},

    Vertex{glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec3( 0.0f, 0.0f, -1.0), glm::vec2(0.0f, 0.0f)},
    Vertex{glm::vec3( 0.0f, 0.8f,  0.0f), glm::vec3( 0.0f, 0.0f, -1.0), glm::vec2(0.5f, 1.0f)},
    Vertex{glm::vec3( 0.5f, 0.0f, -0.5f), glm::vec3( 0.0f, 0.0f, -1.0), glm::vec2(1.0f, 0.0f)},
                                                      
    Vertex{glm::vec3( 0.5f, 0.0f, -0.5f), glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
    Vertex{glm::vec3( 0.0f, 0.8f,  0.0f), glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec2(0.5f, 1.0f)},
    Vertex{glm::vec3( 0.5f, 0.0f,  0.5f), glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
                                                      
    Vertex{glm::vec3( 0.5f, 0.0f,  0.5f), glm::vec3( 0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
    Vertex{glm::vec3( 0.0f, 0.8f,  0.0f), glm::vec3( 0.0f, 0.0f, 1.0f), glm::vec2(0.5f, 1.0f)},
    Vertex{glm::vec3(-0.5f, 0.0f,  0.5f), glm::vec3( 0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)},
    
    Vertex{glm::vec3(-0.5f, 0.0f,  0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
    Vertex{glm::vec3( 0.0f, 0.8f,  0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.5f, 1.0f)},
    Vertex{glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)}    // (10)Lower right corner
    };

    // Indices combinations that make up shape
    vector<GLuint> indices =
    {
        0, 1, 2,
        0, 2, 3,
        4, 5, 6,
        7, 8, 9,
        10, 11, 12,
        13, 14, 15
    };

    // Returns a new Mesh
    Mesh pyramid(vertices, indices);
    return pyramid;
}



// glfw: whenever the mouse moves, this callback is called
// _______________________________________________________
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    // Temp variables to hold current mouse position
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    // Check if this is the first time the mouse has moved since start of program
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // Calculates the difference between current position and last known mouse position
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    // Sets variable for last x and y positions to current x and y position
    lastX = xpos;
    lastY = ypos;

    // Sets the sensitivity for mouse movement
    float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // Sets the new position for the camera focus position
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}



// Handles the mouse scroll wheel events
// _____________________________________
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    cout << "Mouse Scroll " << yoffset << endl;
    // Increase movement speed in window
    speed -= yoffset * 4;
    // Boundaries for speed
    if (speed > 200.0f)
        speed = 200.0f;
    if (speed < 5.0f)
        speed = 5.0f;
}