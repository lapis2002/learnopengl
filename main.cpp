#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void checkCompileShader(unsigned int shader);
void checkCompileProgram(unsigned int program);

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

float firstTriangle[] = {
        -0.9f, -0.5f, 0.0f,  // left 
        -0.0f, -0.5f, 0.0f,  // right
        -0.45f, 0.5f, 0.0f,  // top 
};
float secondTriangle[] = {
    0.0f, -0.5f, 0.0f,  // left
    0.9f, -0.5f, 0.0f,  // right
    0.45f, 0.5f, 0.0f   // top 
};

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* firstFragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

const char* secondFragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(1.0f, 0.8f, 0.1f, 1.0f);\n"
"}\0";

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
    //create vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // attach shader source code to the shader object and compile the shader
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    checkCompileShader(vertexShader);

    // create fragment shader
    // first fragment shader
    unsigned int firstFragmentShader;
    firstFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(firstFragmentShader, 1, &firstFragmentShaderSource, NULL);
    glCompileShader(firstFragmentShader);

    // second fragment shader
    unsigned int secondFragmentShader;
    secondFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(secondFragmentShader, 1, &secondFragmentShaderSource, NULL);
    glCompileShader(secondFragmentShader);

    // First shader program
    unsigned int firstShaderProgram;
    firstShaderProgram = glCreateProgram();  // ID reference

    glAttachShader(firstShaderProgram, vertexShader);
    glAttachShader(firstShaderProgram, firstFragmentShader);
    glLinkProgram(firstShaderProgram);

    checkCompileProgram(firstShaderProgram);

    // second shader program
    unsigned int secondShaderProgram;
    secondShaderProgram = glCreateProgram();  // ID reference

    glAttachShader(secondShaderProgram, vertexShader);
    glAttachShader(secondShaderProgram, secondFragmentShader);
    glLinkProgram(secondShaderProgram);

    checkCompileProgram(secondShaderProgram);

    glUseProgram(firstShaderProgram);
    glUseProgram(secondShaderProgram);

    // delete shader after use
    glDeleteShader(vertexShader);
    glDeleteShader(firstFragmentShader);

    unsigned int VBO[2], VAO[2];

    // First triangle
    glGenVertexArrays(1, &VAO[0]);
    glGenBuffers(1, &VBO[0]);

    glBindVertexArray(VAO[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(firstTriangle), firstTriangle, GL_STATIC_DRAW);

    // set the vertex attribute pointer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // Second triangle
    glGenVertexArrays(1, &VAO[1]);
    glGenBuffers(1, &VBO[1]);

    glBindVertexArray(VAO[1]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(secondTriangle), secondTriangle, GL_STATIC_DRAW);

    // set the vertex attribute pointer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // rendering
        // clear the color buffer, the entire color buffer will be filled with the color as configured by glClearColor. 
        glClearColor(0.2f, 0.3f, 0.3f, 0.1f);   // !state-setting function
        glClear(GL_COLOR_BUFFER_BIT);           // !state-using function

        // use shader program to render object
        glUseProgram(firstShaderProgram);

        // first triangle
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glUseProgram(secondShaderProgram);
        // second triangle
        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);

        // checks if any events are triggered (like keyboard input or mouse movement events), 
        // updates the window state 
        // calls the corresponding functions (can be registered via callback methods)
        glfwPollEvents();
    }

    //  de-allocate all resources
    glDeleteVertexArrays(1, &VAO[0]);
    glDeleteBuffers(1, &VBO[0]);
    glDeleteVertexArrays(1, &VAO[1]);
    glDeleteBuffers(1, &VBO[1]);
    glDeleteProgram(firstShaderProgram);
    glDeleteProgram(secondShaderProgram);

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

// -------------------------------------------------------------------------------------------------------------------------------------
void checkCompileShader(unsigned int shader) {
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

// -------------------------------------------------------------------------------------------------------------------------------------
void checkCompileProgram(unsigned int program) {
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "ERROR::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}