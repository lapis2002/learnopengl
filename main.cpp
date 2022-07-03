#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include <Shader.h>
#include <Camera.h>
#include <Model.h>

#define NR_POINT_LIGHTS 4  

unsigned int loadTexture(const char* path);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

float cubeVertices[] = {
    // positions          // texture Coords
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};
float planeVertices[] = {
    // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
     5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
    -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

     5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
     5.0f, -0.5f, -5.0f,  2.0f, 2.0f
};


// camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;


// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(0.2f, -0.5f, 1.5f);
glm::vec3 lightDir(0.2f, 0.5f, -1.5f);

struct LightPoint {
    LightPoint(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 position, float constant, float linear, float quadratic) : ambient(ambient),
        diffuse(diffuse),
        specular(specular),
        position(position),
        constant(constant),
        linear(linear),
        quadratic(quadratic) {};
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    glm::vec3 position;

    float constant;
    float linear;
    float quadratic;
};

LightPoint lightPoints[NR_POINT_LIGHTS] = {
     LightPoint(glm::vec3(1.f, 0.5f, 0.2f),   glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(1.7f,  0.2f,  2.f), 1.f, 0.9f, 1.6f),
     LightPoint(glm::vec3(0.2f, 0.8f, 0.6f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(2.3f, -3.3f, -4.f), 1.f, 0.7f, 1.8f),
     LightPoint(glm::vec3(0.7f, 0.7f, 0.1f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(-4.f,  2.f, -7.f),  1.f, 0.09f, 0.032f),
     LightPoint(glm::vec3(0.2f, 0.6f, 0.8f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(0.f,  2.f, -3.f),   1.f, 0.14f, 0.07f)
};

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // for macOS

    // create Window Obj
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "ERROR::Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Pass GLAD the function to load the address of the OpenGL function pointers which is OS-specific.
    // glfwGetProcAddress defining the correct function based on which OS that is compiling. 
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "ERROR::Failed to initialize GLAD!" << std::endl;
        return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // -------------------------------------------------------------------------------------------------------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // configure global opengl state
    // -------------------------------------------------------------------------------------------------------------------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    stbi_set_flip_vertically_on_load(true); // OpenGL 0.0 coordinate on the y-axis to be on the bottom side but img usually top of y axis

    // SHADER
    // -------------------------------------------------------------------------------------------------------------------------------------
    Shader shader = Shader("shaders/depthTesting.vert", "shaders/depthTesting.frag");
    Shader stencilShader = Shader("shaders/depthTesting.vert", "shaders/singleColorShader.frag");

    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    // load textures
    // -------------
    unsigned int cubeTexture = loadTexture("resources/textures/marble.jpg");
    unsigned int floorTexture = loadTexture("resources/textures/metal.png");

    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("texture1", 0);

    // -------------------------------------------------------------------------------------------------------------------------------------
    // draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // -------------------------------------------------------------------------------------------------------------------------------------

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //!!  NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    // unbind VAO
    glBindVertexArray(0);

    // -------------------------------------------------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // -------------------------------------------------------------------------------------------------------------------------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // rendering
        // -------------------------------------------------------------------------------------------------------------------------------------
        // clear the color buffer, the entire color buffer will be filled with the color as configured by glClearColor. 
        glClearColor(0.01f, 0.01f, 0.01f, 0.6f);                                                // !state-setting function
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);             // !state-using function

        glStencilMask(0x00);
        
        shader.use();
        glm::mat4 modelMat = glm::mat4(1.0f);
        glm::mat4 viewMat = camera.getViewMatrix();
        glm::mat4 projMat = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(WIDTH) / static_cast<float>(HEIGHT), 0.1f, 100.0f);
        shader.setMat4("viewMat", viewMat);
        shader.setMat4("projMat", projMat);
        
        // floor
        shader.setMat4("modelMat", modelMat);

        glStencilMask(0x00);
        // floor
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        shader.setMat4("modelMat", modelMat);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // 1st. render pass, draw objects as normal, writing to the stencil buffer
        // --------------------------------------------------------------------
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        // cubes
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        modelMat = glm::translate(modelMat, glm::vec3(-1.0f, 0.0f, -1.0f));
        shader.setMat4("modelMat", modelMat);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(2.0f, 0.0f, 0.0f));
        shader.setMat4("modelMat", modelMat);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
        // Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are not drawn, thus only drawing 
        // the objects' size differences, making it look like borders.
        // -----------------------------------------------------------------------------------------------------------------------------
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        stencilShader.use();
        stencilShader.setMat4("viewMat", viewMat);
        stencilShader.setMat4("projMat", projMat);

        float scale = 1.1f;
        // cubes
        glBindVertexArray(cubeVAO);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(-1.0f, 0.0f, -1.0f));
        modelMat = glm::scale(modelMat, glm::vec3(scale, scale, scale));
        stencilShader.setMat4("modelMat", modelMat);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(2.0f, 0.0f, 0.0f));
        modelMat = glm::scale(modelMat, glm::vec3(scale, scale, scale));
        stencilShader.setMat4("modelMat", modelMat);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glEnable(GL_DEPTH_TEST);

        glBindVertexArray(0);
        

        // swap the color buffer
        // -------------------------------------------------------------------------------------------------------------------------------------
        // swap the back buffer to the front buffer so the image can be displayed without still being rendered to, 
        // removing all the aforementioned artifacts. 
        glfwSwapBuffers(window);

        // checks if any events are triggered (like keyboard input or mouse movement events), 
        // updates the window state 
        // calls the corresponding functions (can be registered via callback methods)
        glfwPollEvents();
    }

    //  de-allocate all resources
    // -------------------------------------------------------------------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &planeVBO);

    glfwTerminate();
    return 0;
}

// -------------------------------------------------------------------------------------------------------------------------------------
unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cerr << "ERROR::TEXTURE::CANNOT LOAD TEXTURE" << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}


// -------------------------------------------------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// -------------------------------------------------------------------------------------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn) {
    float xPos = static_cast<float> (xPosIn);
    float yPos = static_cast<float> (yPosIn);

    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;

        firstMouse = false;
    }

    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos; // reversed since y-coordinates go from bottom to top

    lastX = xPos;
    lastY = yPos;

    camera.processMouseMovement(xOffset, yOffset);
}

// -------------------------------------------------------------------------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.processMouseScroll(static_cast<float>(yoffset));
}

// -------------------------------------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)  glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(CAMERA_MOVEMENT::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(CAMERA_MOVEMENT::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(CAMERA_MOVEMENT::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(CAMERA_MOVEMENT::RIGHT, deltaTime);
}
