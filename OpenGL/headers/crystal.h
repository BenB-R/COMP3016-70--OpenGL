#ifndef CRYSTAL_H
#define CRYSTAL_H

#include <glm/glm.hpp>
#include "model.h"

class Crystal {
public:
    glm::vec3 position; // Position of the crystal
    Model model;       // The 3D model of the crystal

    // Constructor
    Crystal(const glm::vec3& pos, const Model& mod) : position(pos), model(mod) {}

    // Function to draw the crystal
    void Draw(Shader& shader) {
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
        shader.setMat4("model", modelMatrix);
        model.Draw(shader);
    }
};

#endif // CRYSTAL_H
