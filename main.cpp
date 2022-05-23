#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <iostream>

#include <shader.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;


float vertices[] = {
    // positions         // colors
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
};

/* color changing triangle
float vertices[] = {
    // positions         // colors
     0.5f, -0.5f, 0.0f,  
    -0.5f, -0.5f, 0.0f,  
     0.0f,  0.5f, 0.0f
};
*/

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
   
    unsigned int VBO, VAO;
    // subsequent vertex attribute calls from that point on will be stored inside the VAO
    /*
    VAO stores:
        Calls to glEnableVertexAttribArray or glDisableVertexAttribArray.
        Vertex attribute configurations via glVertexAttribPointer.
        Vertex buffer objects associated with vertex attributes by calls to glVertexAttribPointer.
    */
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

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

    // set the vertex attribute pointer
    // location
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //!!  NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    
    // unbind VAO
    glBindVertexArray(0);

    // draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


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

        // draw the object
        // bind the VAO with the preferred settings before drawing the object 
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

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

