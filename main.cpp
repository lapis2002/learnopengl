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

float vertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

glm::vec3 cubePositions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f,  2.0f, -2.5f),
    glm::vec3(1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
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

    stbi_set_flip_vertically_on_load(true); // OpenGL 0.0 coordinate on the y-axis to be on the bottom side but img usually top of y axis

    // SHADER
    // -------------------------------------------------------------------------------------------------------------------------------------
    Shader lightShader = Shader("shaders/lightSourceShader.vert", "shaders/lightSourceShader.frag");
    Shader cubeShader = Shader("shaders/cubeShader.vert", "shaders/cubeShader.frag");
    Shader modelShader = Shader("shaders/modelLoaderShader.vert", "shaders/modelLoaderShader.frag");

    Model bagModel("resources/objects/backpack/backpack.obj");

    unsigned int VBO, lightVAO;

    // -------------------------------------------------------------------------------------------------------------------------------------
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);              // !buffer type of a vertex buffer
    glBindVertexArray(lightVAO);                                                            // bind Vertex Array Object

    // position attribute
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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
        glClearColor(0.01f, 0.01f, 0.01f, 0.6f);                           // !state-setting function
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);             // !state-using function

        // Transformation matrices
        // Cannot be computed in advance (before the while loop)
        
        
        // -------------------------------------------------------------------------------------------------------------------------------------
        // use shader program to render object

        //lightPos.x = 2.f * sin(static_cast<float>(glfwGetTime()) * 2.f);
        //lightPos.y = cos(static_cast<float>(glfwGetTime()) / 2.0f);

        //lightDir.x = 2.f * sin(static_cast<float>(glfwGetTime()) * 2.f);
        //lightDir.y = cos(static_cast<float>(glfwGetTime()) / 2.0f);

        modelShader.use();

        modelShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        modelShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        modelShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        modelShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        // light
        modelShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        modelShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        modelShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        modelShader.setVec3("light.position", lightPos.x, lightPos.y, lightPos.z);

        modelShader.setVec3("light.direction", lightDir.x, lightDir.y, lightDir.z);

        // light covers distance of 50
        modelShader.setFloat("light.constant", 1.0f);
        modelShader.setFloat("light.linear", 0.07f);
        modelShader.setFloat("light.quadratic", 0.017f);

        modelShader.setFloat("light.innerCutOff", glm::cos(glm::radians(12.5f)));
        modelShader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));

        // light points
        modelShader.setFloat("pointLights[0].constant", lightPoints[0].constant);
        modelShader.setFloat("pointLights[1].constant", lightPoints[1].constant);
        modelShader.setFloat("pointLights[2].constant", lightPoints[2].constant);
        modelShader.setFloat("pointLights[3].constant", lightPoints[3].constant);

        modelShader.setFloat("pointLights[0].linear", lightPoints[0].linear);
        modelShader.setFloat("pointLights[1].linear", lightPoints[1].linear);
        modelShader.setFloat("pointLights[2].linear", lightPoints[2].linear);
        modelShader.setFloat("pointLights[3].linear", lightPoints[3].linear);

        modelShader.setFloat("pointLights[0].quadratic", lightPoints[0].quadratic);
        modelShader.setFloat("pointLights[1].quadratic", lightPoints[1].quadratic);
        modelShader.setFloat("pointLights[2].quadratic", lightPoints[2].quadratic);
        modelShader.setFloat("pointLights[3].quadratic", lightPoints[3].quadratic);

        modelShader.setVec3("pointLights[0].ambient", lightPoints[0].ambient);
        modelShader.setVec3("pointLights[1].ambient", lightPoints[1].ambient);
        modelShader.setVec3("pointLights[2].ambient", lightPoints[2].ambient);
        modelShader.setVec3("pointLights[3].ambient", lightPoints[3].ambient);

        modelShader.setVec3("pointLights[0].diffuse", lightPoints[0].diffuse);
        modelShader.setVec3("pointLights[1].diffuse", lightPoints[1].diffuse);
        modelShader.setVec3("pointLights[2].diffuse", lightPoints[2].diffuse);
        modelShader.setVec3("pointLights[3].diffuse", lightPoints[3].diffuse);

        modelShader.setVec3("pointLights[0].specular", lightPoints[0].specular);
        modelShader.setVec3("pointLights[1].specular", lightPoints[1].specular);
        modelShader.setVec3("pointLights[2].specular", lightPoints[2].specular);
        modelShader.setVec3("pointLights[3].specular", lightPoints[3].specular);

        modelShader.setVec3("pointLights[0].position", lightPoints[0].position);
        modelShader.setVec3("pointLights[1].position", lightPoints[1].position);
        modelShader.setVec3("pointLights[2].position", lightPoints[2].position);
        modelShader.setVec3("pointLights[3].position", lightPoints[3].position);

        // material
        modelShader.setFloat("material.shininess", 16.f);

        modelShader.setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);

        glm::mat4 viewMat = camera.getViewMatrix();
        glm::mat4 projMat = glm::perspective(glm::radians(camera.Zoom), static_cast<float> (WIDTH) / static_cast<float> (HEIGHT), 0.1f, 100.f);
        glm::mat4 modelMat = glm::mat4(1.f);

        modelMat = glm::translate(modelMat, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        modelMat = glm::scale(modelMat, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down

        modelShader.setMat4("viewMat", viewMat);
        modelShader.setMat4("projMat", projMat);
        modelShader.setMat4("modelMat", modelMat);

        bagModel.draw(modelShader);

        // -------------------------------------------------------------------------------------------------------------------------------------

        

        lightShader.use();
        lightShader.setMat4("viewMat", viewMat);
        lightShader.setMat4("projMat", projMat);

        /*
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        */


        for (unsigned int i = 0; i < 4; i++)
        {
            modelMat = glm::mat4(1.0f);
            modelMat = glm::translate(modelMat, lightPoints[i].position);
            modelMat = glm::scale(modelMat, glm::vec3(0.4f)); // a smaller cube
            float angle = 20.0f * i;
            modelMat = glm::rotate(modelMat, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            lightShader.setMat4("modelMat", modelMat);
            lightShader.setVec3("lightColor", lightPoints[i].ambient);

            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


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
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
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
