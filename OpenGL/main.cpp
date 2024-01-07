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
void PrintMatrix(const glm::mat4& mat);
void CheckShaderCompilation(GLuint shader, std::string type);

#pragma region Settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// Object Initialisation
Camera camera(glm::vec3(25.0f, 45.0f, 55.0f)); // Scene starts in cave at this location (20, 45, 55
bool firstMouse = true;

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
    Shader animShader("shaders/anim_vertex_shader.vs", "shaders/anim_fragment_shader.fs");

    Model crystal("models/crystal/crystal.obj");
    Model mineStruct1("models/mineshaft/mineshaft_structure1.obj");
    Model pick("models/pick/pick.dae");
    
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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

    // Load second texture
    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    unsigned char* data2 = stbi_load("textures/cracks.png", &width, &height, &nrChannels, 0);
    if (data2)
    {
        std::cout << "crack texture loaded" << std::endl;
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load second texture" << std::endl;
    }
    stbi_image_free(data2);


    CaveGenerator cave(75, 50, 75, 0.5f);
    cave.generateCave();
    cave.generateCrystals();
    float rotationAngle = 0.0f;

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
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set up camera and projection matrices (common for both crystals and cave)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
            (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.getViewMatrix();
#pragma region crystal
        // Render Crystals
        for (const auto& pos : cave.getCrystalPositions()) {
            glm::mat4 crystalModelMatrix = glm::mat4(1.0f);

            glm::vec3 offset(0.5f, 0.0f, -0.5f); // Offset so blocks aren't in corners

            glm::vec3 adjustedPos = pos + offset;

            crystalModelMatrix = glm::translate(crystalModelMatrix, adjustedPos);
            crystalModelMatrix = glm::scale(crystalModelMatrix, glm::vec3(0.8f, 0.8f, 0.8f)); // Scale if needed

            crystalShader.use();
            crystalShader.setInt("texture1", 0);

            crystalShader.setMat4("projection", projection);
            crystalShader.setMat4("view", view);
            crystalShader.setMat4("model", crystalModelMatrix);

            crystalShader.setVec3("viewPos", camera.Position);

            crystalShader.setFloat("maxGlowIntensity", 0.5f); // Prevents the glow from becoming too intense
            crystalShader.setFloat("glowVisibilityDistance", 2.0f); // Sets the distance at which the glow is fully visible
            crystalShader.setFloat("glowFactor", 0.001f); // Adjust this factor to control the attenuation of the glow

            crystal.Draw(crystalShader, crystalModelMatrix);
        }
#pragma endregion

#pragma region cave
        // Render Cave
        caveShader.use();

        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        caveShader.setInt("texture1", 0);  // Assuming your shader has a uniform named 'texture1'
        caveShader.setInt("texture2", 1);
        caveShader.setFloat("blendFactor", 0.2f);

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
#pragma endregion

#pragma region mineshaft
        ourShader.use();

        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // Render the loaded model (mine structure)
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(25.0f, 40.0f, 22.0f)); // Adjust the position as needed
        model = glm::scale(model, glm::vec3(0.75f, 0.75f, 0.75f)); // Adjust the scale as needed
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        ourShader.setMat4("model", model);

        mineStruct1.Draw(ourShader, model);
#pragma endregion

#pragma region pick
        animShader.use();

        animShader.setMat4("projection", projection);
        animShader.setMat4("view", view);

        rotationAngle = glm::sin(glfwGetTime()) * 45.0f; // Oscillates between -45 and 45 degrees

        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle), glm::vec3(0.0, 0.0, 1.0));
        glm::mat4 pickModel = glm::translate(glm::mat4(1.0f), glm::vec3(20.6f, 42.5f, 27.5f));
        pickModel = glm::scale(pickModel, glm::vec3(0.5f, 0.5f, 0.5f));
        pickModel = glm::rotate(pickModel, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        pickModel = pickModel * rotationMatrix;
        animShader.setMat4("model", pickModel);

        pick.Draw(animShader, pickModel);
#pragma endregion

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error: " << err << std::endl;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
#pragma endregion Render Loop

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

void PrintMatrix(const glm::mat4& mat) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << mat[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

void CheckShaderCompilation(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "Shader compilation error of type: " << type << "\n" << infoLog << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "Shader linking error: \n" << infoLog << std::endl;
        }
    }
}
