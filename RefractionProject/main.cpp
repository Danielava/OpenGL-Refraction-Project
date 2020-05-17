#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "shader.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //For matrix transformations
#include "model.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
unsigned int loadCubemap(vector<std::string> faces);
unsigned int setupNormalMapFrontVAO(unsigned int VAO);

// A 3D cube
float skyboxVertices[] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

/*
    Will use this structure for the normal and depth maps (front & back).
    It's like a screen we can render the texture to (save values on),
    to then pick the values with uv-coordinates.
*/
float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

//Paths to faces of the cubemap
/*
vector<std::string> faces
{
    "skybox/space/right.jpg",
    "skybox/space/left.jpg",
    "skybox/space/top.jpg",
    "skybox/space/bottom.jpg",
    "skybox/space/front.jpg",
    "skybox/space/back.jpg"
};*/

/*
vector<std::string> faces
{
    "skybox/space2/right.jpg",
    "skybox/space2/left.jpg",
    "skybox/space2/top.jpg",
    "skybox/space2/bottom.jpg",
    "skybox/space2/front.jpg",
    "skybox/space2/back.jpg"
};
*/


vector<std::string> faces
{
    "skybox/sky/right.jpg",
    "skybox/sky/left.jpg",
    "skybox/sky/top.jpg",
    "skybox/sky/bottom.jpg",
    "skybox/sky/front.jpg",
    "skybox/sky/back.jpg"
};

//vec3 color(0.7f, 0.5f, 0.2f);
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

const int SCREEN_HEIGHT = 1200;
const int SCREEN_WIDTH = 1600;

int main()
{
    GLFWwindow* window;
    
    // Initialize the library
    if(!glfwInit())
        return -1;

    // Define version and compatibility settings
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    /*
        MacOS retina bug: The glfwCreateWindow function creates a window with double the dimensions you give it
        for some reason. That's why I multiplied it with 0.5. This will correctly create a window with your given
        values for width and height.
        When creating the framebuffer and other stuff which uses SCREEN_WIDTH and SCREEN_HEIGHT variables,
        there is no need to multiply them with 0.5. Only when first creating the window with glfwCreateWindow().
        
        NOTE: If this application is run on another operating system than macOS, you must probably remove the 0.5 from this function.
    */
    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(SCREEN_WIDTH*0.5, SCREEN_HEIGHT*0.5, "Hello World", NULL, NULL);
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
    //glViewport(0, 0, SCREEN_WIDTH*2.0, SCREEN_HEIGHT*2.0);
    glEnable(GL_DEPTH_TEST);
    //Create a shader using our shader class
    Shader shader("shaders/objVshader.txt", "shaders/objFshader.txt");
    Shader skyboxShader("shaders/skyboxVshader.txt", "shaders/skyboxFshader.txt");
    Model catModel("models/cat/cat.obj");
    //Model backPack("models/backpack/backpack.obj");
    
    //VAO and VBO for skybox
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    unsigned int cubemapTexture = loadCubemap(faces);
    skyboxShader.use();
    //set the int
    glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0); //0 represents GL_TEXTURE0
    
    glUseProgram(shader.ID);
    /*
        Drawing in 3D.
        1. We need a model matrix, which turns local coordinates to world coordinates.
     
        2. View matrix: We put the coordinates to cameras point of view before projecting them to clip coordinates.
     
        3. Projection matrix (perspective or orthographic)
    */
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(180.0f), (float)SCREEN_WIDTH/(float)SCREEN_HEIGHT, 0.1f, 1000.0f); //first param is field of view
    
    shader.use(); //Activate the shader before setting its values! important
    //Set the matrices to vertice shaders uniform variables
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, &projection[0][0]);
    //Put in the uniform variables inside the shaders
    glUniform1i(glGetUniformLocation(shader.ID, "skybox"), 0);
    glUniform3fv(glGetUniformLocation(shader.ID, "cameraPos"), 1, &cameraPos[0]); //param2 = 1 because we are putting in 1 vec3
    
    /*
     Generate your normal and depth textures here.
     Create a VAO for each of the settings. When you have done that you can simply call
     that VAO with glBindVertexArray(thisVAO) and draw => catModel.Draw() and the catmodel will
     be drawn on that texture.
    */
    /*
    unsigned int normalFrontVAO, normalFrontFB; //FB stands for framebuffer
    glGenVertexArrays(1, &normalFrontVAO);
    glBindVertexArray(normalFrontVAO);
    normalFrontFB = setupNormalMapFrontVAO(normalFrontVAO); //Put the normalFrontTexture into refraction shader uniform variable
    Shader normalShader("shaders/normVshader.txt", "shaders/normFshader.txt");
    */
    Shader normalShader("shaders/normVshader.txt", "shaders/normFshader.txt");
    normalShader.use(); //Remember to activate it first!
    glUniformMatrix4fv(glGetUniformLocation(normalShader.ID, "projection"), 1, GL_FALSE, &projection[0][0]);
    
    //Framebuffer for normal front texture
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a color attachment texture
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    /*
     Here we must bind the texture, and then if we want to pass the
     texture to the default shader we do.
     1. shader.use();
     2. glUniform1i(glGetUniformLocation(shader.ID, "normalFrontTexture"), 0??);
    */
    glActiveTexture(GL_TEXTURE0 + 0); //Default at 0, here we just set the texture unit of the shader, this unit (0) is used later in glUniform1i(glGetUniformLocation(shader.ID, "normalFrontTexture"), 0); when we want to assign the shader uniform variable to THIS texture-
        //Also GL_TEXTURE0 + 2 = GL_TEXTURE2
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer); //Just remember the openGL process is always, unsigned int X, glgenerateObj(1,&X), glBindObj(GL_BUILT_IN_VALUE_ X); Then you have created your object
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); //Craete the texture, no data inside=NULL
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    
    //glViewport(0, 0, 1800.0, 1600.0);
    //Attach your texture to the framebuffer's color buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    /*------------------------------------------------------------------------------*/
    
    normalShader.use();
    //Framebuffer for normal back texture
    unsigned int framebuffer2;
    glGenFramebuffers(1, &framebuffer2);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer2);
    // create a color attachment texture
    unsigned int textureColorbuffer2;
    glGenTextures(1, &textureColorbuffer2);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    //Attach your texture to the framebuffer's color buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer2, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo2;
    glGenRenderbuffers(1, &rbo2);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo2);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo2); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    
    /*------------------------------------------------------------------------------*/
    
    /*
        Here we set the normalFrontTexture in default shader to the framebuffers texture.
    */
    shader.use();
    glUniform1i(glGetUniformLocation(shader.ID, "normalFrontTexture"), 0); //0 is the texture unit. We set the framebuffer texture to unit 0 (which openGL does automatically).
    glUniform1i(glGetUniformLocation(shader.ID, "normalBackTexture"), 1);
    
    
    // Loop until the user closes the window
    
    while(!glfwWindowShouldClose(window))
    {
        glEnable(GL_DEPTH_TEST);
        //Input
        processInput(window);
        //glViewport(0,0,800*2,600*2);
        //Camera settings
        const float radius = 150.0f; //Lower this to make object come closer
        float speed = 0.2f;
        float camX = sin(glfwGetTime()*speed) * radius;
        float camZ = cos(glfwGetTime()*speed) * radius;
        glm::mat4 view;
        
        //TURN THIS ON FOR MANUAL CAMERA MOVEMENT
        view = glm::lookAt(40.0f*cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));    // it's a bit too big for our scene, so scale it down
        
        // first pass Render the front normals
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        
        //glViewport(0,0,800,600);
        //glViewport(0,0,100,100);
        //glViewport(0, 0, SCREEN_WIDTH*2, SCREEN_HEIGHT*2);
        glEnable(GL_DEPTH_TEST);
        //glDisable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glClearColor(1.0f, 0.1f, 0.1f, 1.0f);
        //glViewport(0, 0, 800.0, 600.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
        //Draw the catModel with normalShader and it will end up in our framebuffer
        //FATAL ERROR!!! I MUST DO SHADER.USE() BEFORE PUTTING IN THE VARIABLES!!!!!
        normalShader.use();
        glUniformMatrix4fv(glGetUniformLocation(normalShader.ID, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(normalShader.ID, "model"), 1, GL_FALSE, &model[0][0]);
        //glDepthFunc(GL_GREATER);
        catModel.Draw(normalShader, false);
        
        //Render the back normals
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer2);
        glEnable(GL_DEPTH_TEST);
        glClearDepth(0.0f);
        glDepthFunc(GL_GREATER);
        glClearColor(1.0f, 0.1f, 0.1f, 1.0f); //Depth on this is probably 0
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
        //glClearDepth(0.0f);
        //glDepthFunc(GL_GREATER);
        normalShader.use();
        glUniformMatrix4fv(glGetUniformLocation(normalShader.ID, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(normalShader.ID, "model"), 1, GL_FALSE, &model[0][0]);
        //glDepthFunc(GL_GREATER);
        catModel.Draw(normalShader, true);
        
        //Second pass
        glClearDepth(1.0f); //This function apparently sets some setting on all depth buffers..
        glBindFramebuffer(GL_FRAMEBUFFER, 0); //0 = our main screen (default screen)
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        //glDisable(GL_DEPTH_TEST);
        //glDepthFunc(GL_LESS);
        glClearColor(0.0f, 0.1f, 0.0f, 0.3f);
        glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
        shader.use();//glUseProgram(shader.ID);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        glUniform3fv(glGetUniformLocation(shader.ID, "cameraPos"), 1, &cameraPos[0]); //Update uniform cameraPos in fragment shader every frame
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, &model[0][0]);
        //glUniform1i(glGetUniformLocation(shader.ID, "normalFrontTexture"), 0);
        //glUniform1i(glGetUniformLocation(shader.ID, "normalFrontTexture"), 0);
        catModel.Draw(shader, false);
        /*
           AFTER SETTING THE MODEL, VIEW, PROJ MATRICES => RENDER YOUR TEXTURES
           1. Front normals
        */
        /*
        glBindVertexArray(normalFrontVAO); //Bind the corresponding VAO to use its settings.
        glBindFramebuffer(GL_FRAMEBUFFER, normalFrontFB); //Bind them before drawing on them
        //Turn on the shader that visualizes the normals
        normalShader.use();
        //Assign the relevant uniform variables in the shader
        glUniformMatrix4fv(glGetUniformLocation(normalShader.ID, "model"), 1, GL_FALSE, &model[0][0]);
        //Draw your model
        catModel.Draw(normalShader);
        //The cat model has been drawn on the texture, repeat this process for the other side as well
        */
        //TURN THIS ON FOR AUTOMATIC CAMERA ROTATION AROUND OBJECT
        /*
        glm::vec3 rot = glm::vec3(camX, 0.0f, camZ);
        view = glm::lookAt(rot, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniform3fv(glGetUniformLocation(shader.ID, "cameraPos"), 1, &rot[0]); //Third param is probably wrong..
        */
        
        //CODE FOR CUBE
        /*
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(20.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        //view = glm::translate(view, glm::vec3(1.5f*cos(time*0.2f), 1.5f*sin(time*0.2f), -100.0f)); //Moves the scene back a bit
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, &model[0][0]);
        */
        
        //THIS IS FOR 3D MODEL!
         // render the loaded model
        /*
         glm::mat4 model = glm::mat4(1.0f);
         model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
         model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));    // it's a bit too big for our scene, so scale it down
         shader.setMat4("model", model);
        */
        
        //Before drawing on your main screen, bind the default VAO and change to the default frameBuffer
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //glBindVertexArray(VAO);
        
        /*
         Remember the vertices have gone through the shaders and are being colored
         so when finally drawing the cube, the shaders will have done the job before it's drawn.
        */
        //glBindVertexArray(VAO); //Put this before Drawing
        //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); //To draw wireframe
        //catModel.Draw(shader);
        //backPack.Draw(shader);
        //glDrawArrays(GL_TRIANGLES, 0, 36); //36 is # vertices
        /*
        shader.use();
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture); //??? what does this do?
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        */
        
        // draw skybox as last
        glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
        //fov should be larger for cubemap
        projection = glm::perspective(glm::radians(5000.0f), (float)SCREEN_WIDTH/(float)SCREEN_HEIGHT, 0.1f, 1000.0f);
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, &projection[0][0]);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture); //??? what does this do?
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default
        
        // Swap front and back buffers
        glfwSwapBuffers(window);
        // Poll for and process events like joystick/inputs mouse movement etc
        glfwPollEvents();
    }
    
    //De-allocate recourses
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    //glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width
    // and height will be significantly larger than specified on retina displays
    //glViewport(0, 0, width*2, height*2);
    //std::cout << width << " " << height << std::endl;
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    const float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

unsigned int loadCubemap(vector<std::string> faces) {
    //Texture for cubemap
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    
    int width, height, nrChannels;
    unsigned char *data;
    for(GLuint i = 0; i < faces.size(); i++) {
        data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if(data) {
            //If you use sky, change GL_RGBA to GL_RGB
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] <<std::endl;
            stbi_image_free(data);
        }
    }
    //Settings for cubemap
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    return textureID;
}

/*
unsigned int createDepthMapFront() {
    GLuint depthrenderbuffer;
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
}*/
