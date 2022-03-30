// GLApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
//GLM Mathematics
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

//SOIL FOR Image
#include "SOIL2/SOIL2.h"
//Other Includes
#include "Shader.h"
#include "Camera.h"

const GLint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);

void DoMovement();

Camera camera(glm::vec3(0.0f, 0.0f, 0.3f));
GLfloat lastX = WIDTH / 2.0f;
GLfloat lastY = WIDTH / 2.0f;
bool keys[1024];
bool firstMouse = true;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
    // Initialize GLFW
    glfwInit();

    // Required/Changeabel properties or options for Glfw
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Creating a GLFWwindow Object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Test", nullptr, nullptr);

    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

    if (nullptr == window)
    {
        std::cout << "Falied to create GLFW window" << std::endl;
        glfwTerminate();

        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //setting this to true so glew knows to use modern approch to retriveing function pointers and externsions 
    glewExperimental = GL_TRUE;
    // intitializing glew to setup opengel Function pointers 
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed To initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }
    // defining viewport dimensions 
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    glEnable(GL_DEPTH_TEST);//to enable depth

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //Build and compile our shaderProgram
    Shader LightingShader("res/shaders/Lighting.vs", "res/shaders/Lighting.frag");
    Shader LampShader("res/shaders/Lamp.vs", "res/shaders/Lamp.frag");


    // Set up vertex data (and buffer(s)) and attribute pointers
    // use with Perspective Projection
    GLfloat vertices[] = {
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
        -0.5f,  0.5f, -0.5f
    };

    

    GLuint VBO, boxVAO;
    glGenVertexArrays(1, &boxVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(boxVAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //position arttribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); //UnBind vao

    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);

    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //position arttribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); //UnBind vao

    glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

    

    //Game Loop
    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //check for Events or Input
        glfwPollEvents();
        DoMovement();
        //Handle game logic 

        //Render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        LightingShader.Use();
        GLint objectColorLoc = glGetUniformLocation(LightingShader.Program, "objectColor");
        GLint lightColorLoc = glGetUniformLocation(LightingShader.Program, "lightColor");
        glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
        glUniform3f(lightColorLoc, 1.0f, 0.5f, 1.0f);

        glm::mat4 view = glm::mat4(1.0f);
        view = camera.GetViewMatrix();

        GLint modelLoc = glGetUniformLocation(LightingShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(LightingShader.Program, "view");
        GLint projLoc = glGetUniformLocation(LightingShader.Program, "projection");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(boxVAO);
        glm::mat4 model = glm::mat4(100.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);


        LampShader.Use();

        modelLoc = glGetUniformLocation(LampShader.Program, "model");
        viewLoc = glGetUniformLocation(LampShader.Program, "view");
        projLoc = glGetUniformLocation(LampShader.Program, "projection");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        //Draw OPENGL Swaping the screen buffers 
        glfwSwapBuffers(window);
    }

    //De-allocating the resources
    glDeleteVertexArrays(1, &boxVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    //terminating GlFW, clearing all the resources allocated by glfw.
    glfwTerminate();
    return EXIT_SUCCESS;
}

// Moves/alters the camera positions based on user input
void DoMovement()
{
    // Camera controls
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }

    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }

    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }

    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            keys[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            keys[key] = false;
        }
    }
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}
