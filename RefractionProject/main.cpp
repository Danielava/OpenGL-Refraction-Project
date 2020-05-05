#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "shader.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //For matrix transformations


void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// A 3D cube
float vertices[] = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
};

//vec3 color(0.7f, 0.5f, 0.2f);

int main()
{
    GLFWwindow* window;
    const int SCREEN_HEIGHT = 640;
    const int SCREEN_WIDTH = 480;

    // Initialize the library
    if(!glfwInit())
        return -1;

    // Define version and compatibility settings
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(SCREEN_HEIGHT, SCREEN_WIDTH, "Hello World", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Mathe the window's context current
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // Initialize the OpenGL API with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    glEnable(GL_DEPTH_TEST);
    //Create a shader using our shader class
    Shader shader("vshader.txt", "fshader.txt");
  
    unsigned int VBO, VAO;
    //Generate a Vertex buffer object (only 1 as described by param1)
    glGenBuffers(1, &VBO);
    //Adding a VAO made the render work
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //From now on, any buffer function call on the GL_ARRAY_BUFFER (which holds the VBO), will configure the current VBO.
    //Put data on the GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //Linking vertex attributes to the input of vertex shader (first param is location=0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Initialize the OpenGL API with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    /*
        Drawing in 3D.
        1. We need a model matrix, which turns local coordinates to world coordinates.
     
        2. View matrix: We put the coordinates to cameras point of view before projecting them to clip coordinates.
     
        3. Projection matrix (perspective or orthographic)
    */
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(180.0f), (float)SCREEN_HEIGHT/(float)SCREEN_WIDTH, 0.1f, 200.0f); //first param is field of view
    
    shader.use(); //Activate the shader before setting its values! important
    //Set the matrices to vertice shaders uniform variables
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, &projection[0][0]);
    
    // Loop until the user closes the window
    while(!glfwWindowShouldClose(window))
    {
        // Render here!
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.1f, 0.0f, 0.3f);
        glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader.ID);
        
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view  = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime()*50.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -100.0f)); //Moves the scene back a bit
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, &view[0][0]);
        
        /*
         Remember the vertices have gone through the shaders and are being colored
         so when finally drawing the cube, the shaders will have done the job before it's drawn.
        */
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36); //36 is # vertices
        
        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events like joystick/inputs mouse movement etc
        glfwPollEvents();
    }
    
    //De-allocate recourses
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    //glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width
    // and height will be significantly larger than specified on retina displays
    glViewport(0, 0, width, height);
}
