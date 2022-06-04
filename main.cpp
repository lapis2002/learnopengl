#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include <shader.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

float vertices[] = {
    // positions          // colors           // texture coords
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
};

unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
};

/* color changing triangle
float vertices[] = {
    // positions         // colors
     0.5f, -0.5f, 0.0f,  
    -0.5f, -0.5f, 0.0f,  
     0.0f,  0.5f, 0.0f
};
*/

float texCoords[] = {
    0.0f, 0.0f,  // lower-left corner  
    1.0f, 0.0f,  // lower-right corner
    0.5f, 1.0f   // top-center corner
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

    // Pass GLAD the function to load the address of the OpenGL function pointers which is OS-specific.
    // glfwGetProcAddress defining the correct function based on which OS that is compiling. 
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "ERROR::Failed to initialize GLAD!" << std::endl;
        return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // SHADER
    Shader shaderProgram = Shader("shaders/shader.vert", "shaders/shader.frag");
   
    unsigned int VBO, VAO, EBO;
    // subsequent vertex attribute calls from that point on will be stored inside the VAO
    /*
    VAO stores:
        Calls to glEnableVertexAttribArray or glDisableVertexAttribArray.
        Vertex attribute configurations via glVertexAttribPointer.
        Vertex buffer objects associated with vertex attributes by calls to glVertexAttribPointer.
    */
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // bind Vertex Array Object
    glBindVertexArray(VAO);

    // copy vertices array in buffer for OpenGL to use
    // OpenGL allows us to bind to several buffers at once as long as they have a different buffer type. 
    /*
    GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
    GL_STATIC_DRAW: the data is set only once and used many times.
    GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
    */
    glBindBuffer(GL_ARRAY_BUFFER, VBO);     // !buffer type of a vertex buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);      // copy user-defined data into the currently bound buffer

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // set the vertex attribute pointer
    // location
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)) /* stride 6 offsets*/);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //!!  NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    
    // unbind VAO
    glBindVertexArray(0);

    // draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // TEXTURES
    unsigned int texture1, texture2;

    int texWidth, texHeight, numColorChannels;
    stbi_set_flip_vertically_on_load(true); // OpenGL 0.0 coordinate on the y-axis to be on the bottom side but img usually top of y axis

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data = stbi_load("textures/container.jpg", &texWidth, &texHeight, &numColorChannels, 0);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "ERROR::TEXTURE::CANNOT LOAD TEXTURE" << std::endl;
    }
    stbi_image_free(data);


    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    data = stbi_load("textures/awesomeface.png", &texWidth, &texHeight, &numColorChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0 /* mipmap level (default 0)*/, GL_RGBA /* format to store the texture*/, texWidth, texHeight, 0 /* always */, GL_RGBA /* format of the img */, GL_UNSIGNED_BYTE /* format of the img */, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "ERROR::TEXTURE::CANNOT LOAD TEXTURE" << std::endl;
    }
    stbi_image_free(data);

    shaderProgram.use();
    shaderProgram.setInt("texture1", 0);
    shaderProgram.setInt("texture2", 1);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // rendering
        // clear the color buffer, the entire color buffer will be filled with the color as configured by glClearColor. 
        glClearColor(0.2f, 0.3f, 0.3f, 0.1f);   // !state-setting function
        glClear(GL_COLOR_BUFFER_BIT);           // !state-using function

        // use shader program to render object
        shaderProgram.use();

        // color changing triangle
        /*
        float time = glfwGetTime();
        float green = sin(time) / 2.f + 0.5f;
        int vertexColorLocation = glad_glGetUniformLocation(shaderProgram.id, "outColor");
        glUniform4f(vertexColorLocation, 0.0f, green, 0.0f, 1.0f);
        */

        // bind the texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // transform matrix
        glm::mat4 transfomMat = glm::mat4(1.f);
        transfomMat = glm::translate(transfomMat, glm::vec3(0.5f, -0.5f, 0.f));
        transfomMat = glm::rotate(transfomMat, (float)glfwGetTime(), glm::vec3(0.f, 0.f, 1.f));
        transfomMat = glm::scale(transfomMat, glm::vec3(0.5f, 0.5f, 0.5f));

        unsigned int transformLoc = glGetUniformLocation(shaderProgram.id, "transformMat");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transfomMat));

        // draw the object
        // bind the VAO with the preferred settings before drawing the object 
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // swap the color buffer
        // swap the back buffer to the front buffer so the image can be displayed without still being rendered to, 
        // removing all the aforementioned artifacts. 
        glfwSwapBuffers(window);

        // checks if any events are triggered (like keyboard input or mouse movement events), 
        // updates the window state 
        // calls the corresponding functions (can be registered via callback methods)
        glfwPollEvents();
    }

    //  de-allocate all resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();    
    return 0;
}

// -------------------------------------------------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// -------------------------------------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)  glfwSetWindowShouldClose(window, true);
}

