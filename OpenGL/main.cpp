#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "headers/shader.h"
#include "headers/stb_image.h"
#include "headers/camera.h"
#include "headers/model.h"
#include "headers/CaveGenerator.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, float deltatime);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);


// Object Initialisation
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

bool firstMouse = true;

#pragma region Settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
#pragma endregion

float deltaTime = 0.0f;
float lastFrame = 0.0f;


int main()
{
#pragma region Setup
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Abandoned Mine", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Set callback
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
#pragma endregion

    Shader ourShader("shaders/vertex_shader.vs", "shaders/fragment_shader.fs");
    Shader crystalShader("shaders/light_vertex_shader.vs", "shaders/light_fragment_shader.fs");
    Shader caveShader("shaders/cave_vertex_shader.vs", "shaders/cave_fragment_shader.fs");

    Model woodPlank("models/wood/wood.obj");

    Model crystal("models/crystal/crystal.obj");

    
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load("textures/stone.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        std::cout << "Stone texture loaded" << std::endl;
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    CaveGenerator cave(75, 50, 75, 0.5f);
    cave.generateCave();
    cave.generateCrystals(crystal);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#pragma region Render Loop
    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Calculate delta time for smooth camera movement
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        processInput(window, deltaTime);

        // Rendering commands here
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set up camera and projection matrices (common for both crystals and cave)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
            (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.getViewMatrix();

        // Render Crystals
        for (auto& crystal : cave.crystals) {
            crystal.Draw(crystalShader);
        }

        // Render Cave
        caveShader.use();

        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        caveShader.setInt("texture1", 0);  // Assuming your shader has a uniform named 'texture1'

        caveShader.setMat4("projection", projection);
        caveShader.setMat4("view", view);
        glm::mat4 caveModel = glm::mat4(1.0f); // Apply transformations as needed
        caveShader.setMat4("model", caveModel);

        // Set the color of the cave walls (earthy brownish-grey)
        caveShader.setVec3("objectColor", glm::vec3(0.55f, 0.5f, 0.45f));

        // Set the primary light to mimic an old lantern (dim yellowish light)
        caveShader.setVec3("lightColor", glm::vec3(0.98f, 0.88f, 0.72f));
        caveShader.setVec3("lightDir", glm::normalize(glm::vec3(0.5f, -1.0f, 0.5f)));
        caveShader.setVec3("ambientStrength", glm::vec3(0.15f, 0.15f, 0.15f));

        // Set the secondary light for contrast (softer, cooler light)
        caveShader.setVec3("secondLightDir", glm::normalize(glm::vec3(-0.5f, -1.0f, -0.5f)));
        caveShader.setVec3("secondLightColor", glm::vec3(0.6f, 0.7f, 0.8f));
        caveShader.setVec3("secondAmbientStrength", glm::vec3(0.05f, 0.05f, 0.05f));

        // Render the cave
        cave.render(); // This binds its own VAO and use its own vertex data

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error: " << err << std::endl;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
#pragma endregion Render Loop

#pragma region Clean up

#pragma endregion

    glfwTerminate();
    return 0;
}

// Callback implementation
void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Update the processInput function to handle camera movement
void processInput(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    bool isSprinting = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(FORWARD, deltaTime, isSprinting);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(BACKWARD, deltaTime, isSprinting);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(LEFT, deltaTime, isSprinting);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(RIGHT, deltaTime, isSprinting);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.processKeyboard(UP, deltaTime, isSprinting);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.processKeyboard(DOWN, deltaTime, isSprinting);
}

void mouseCallback(GLFWwindow * window, double xpos, double ypos) {
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // Reversed since y-coordinates range from bottom to top
        lastX = xpos;
        lastY = ypos;

        camera.processMouseMovement(xoffset, yoffset);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.processMouseScroll(static_cast<float>(yoffset));
}

