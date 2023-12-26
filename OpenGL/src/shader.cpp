#include "../headers/shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Constructor for the Shader class
Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    // Retrieve the vertex/fragment source code from file paths
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // Set ifstream objects to throw exceptions if they fail
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // Open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // Read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // Close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // Convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }

    // Convert shader source strings into character array
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // Compile shaders
    unsigned int vertex, fragment;
    // Vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER); // Create a vertex shader
    glShaderSource(vertex, 1, &vShaderCode, NULL); // Attach the vertex shader source code
    glCompileShader(vertex); // Compile the vertex shader
    checkCompileErrors(vertex, "VERTEX"); // Check for vertex shader compile errors

    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER); // Create a fragment shader
    glShaderSource(fragment, 1, &fShaderCode, NULL); // Attach the fragment shader source code
    glCompileShader(fragment); // Compile the fragment shader
    checkCompileErrors(fragment, "FRAGMENT"); // Check for fragment shader compile errors

    // Create shader program and attach compiled shaders
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID); // Link the shaders to create a shader program
    checkCompileErrors(ID, "PROGRAM"); // Check for shader program linking errors

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

// Activate/use the shader program
void Shader::use() const {
    glUseProgram(ID);
}

// Utility function to set a boolean uniform in the shader
void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

// Utility function to set an integer uniform in the shader
void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

// Utility function to set a float uniform in the shader
void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

// Check and log any compilation/linking errors
void Shader::checkCompileErrors(unsigned int shader, const std::string& type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        // Check and log for shader compilation errors
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else {
        // Check and log for shader linking errors
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}
