#include "../headers/CaveGenerator.h"
#include <cmath>
#include <glm/gtc/noise.hpp> // For Perlin noise
#include <iostream>

CaveGenerator::CaveGenerator(int depth, int width, int height, float threshold)
    : depth(depth), width(width), height(height), threshold(threshold) {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
}

CaveGenerator::~CaveGenerator() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void CaveGenerator::generateCave() {
    vertices.clear();
    normals.clear();

    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (isSolid(x, y, z)) {
                    // Add faces for the solid block
                    addFace(x, y, z, glm::vec3(1.0f, 0.0f, 0.0f)); // Right face
                    addFace(x, y, z, glm::vec3(-1.0f, 0.0f, 0.0f)); // Left face
                    addFace(x, y, z, glm::vec3(0.0f, 1.0f, 0.0f)); // Top face
                    addFace(x, y, z, glm::vec3(0.0f, -1.0f, 0.0f)); // Bottom face
                    addFace(x, y, z, glm::vec3(0.0f, 0.0f, 1.0f)); // Front face
                    addFace(x, y, z, glm::vec3(0.0f, 0.0f, -1.0f)); // Back face
                }
            }
        }
    }

    // Update VAO and VBO for vertices and normals
    glBindVertexArray(vao);

    // Bind vertices
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // Bind normals
    GLuint normalVBO;
    glGenBuffers(1, &normalVBO);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    // Unbind buffer and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void CaveGenerator::render() {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);
    glBindVertexArray(0);
}

float CaveGenerator::perlinNoise(int x, int y, int z) {
    float scale = 0.08f; // Adjusted scale for broader features
    float noise = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float persistence = 0.65f; // Adjusted persistence for variation in density
    int octaves = 4; // Keep octaves reasonable to prevent too much fine detail

    for (int i = 0; i < octaves; i++) {
        glm::vec3 pos = glm::vec3(x * scale * frequency, y * scale * frequency, z * scale * frequency);
        noise += amplitude * glm::perlin(pos);
        amplitude *= persistence;
        frequency *= 2.0f;
    }

    return noise;
}

bool CaveGenerator::hasNeighbour(int x, int y, int z, glm::vec3 direction) {
    // Check for boundaries
    if (x + direction.x < 0 || x + direction.x >= width ||
        y + direction.y < 0 || y + direction.y >= height ||
        z + direction.z < 0 || z + direction.z >= depth) {
        return false;
    }
    return isSolid(x + direction.x, y + direction.y, z + direction.z);
}


bool CaveGenerator::isSolid(int x, int y, int z) {
    float threshold = 0.4f; // Lower threshold for more solid blocks
    float noiseValue = perlinNoise(x, y, z);
    return noiseValue > threshold;
}

void CaveGenerator::addFace(int x, int y, int z, glm::vec3 normal) {
    const float blockSize = 1.0f;

    // Determine the starting corner based on the normal
    glm::vec3 startCorner = glm::vec3(x, y, z);

    // Calculate direction vectors based on the face normal
    glm::vec3 right, up;
    if (normal.x > 0) { // Right face
        startCorner += glm::vec3(blockSize, 0, 0);
        right = glm::vec3(0, 0, -blockSize);
        up = glm::vec3(0, blockSize, 0);
    }
    else if (normal.x < 0) { // Left face
        startCorner -= glm::vec3(0, 0, blockSize);
        right = glm::vec3(0, 0, blockSize);
        up = glm::vec3(0, blockSize, 0);
    }
    else if (normal.y > 0) { // Top face
        startCorner += glm::vec3(0, blockSize, -blockSize);
        right = glm::vec3(blockSize, 0, 0);
        up = glm::vec3(0, 0, blockSize);
    }
    else if (normal.y < 0) { // Bottom face
        startCorner -= glm::vec3(0, 0, 0);
        right = glm::vec3(blockSize, 0, 0);
        up = glm::vec3(0, 0, -blockSize);
    }
    else if (normal.z > 0) { // Front face
        startCorner += glm::vec3(0, 0, 0);
        right = glm::vec3(blockSize, 0, 0);
        up = glm::vec3(0, blockSize, 0);
    }
    else if (normal.z < 0) { // Back face
        startCorner -= glm::vec3(-blockSize, 0, blockSize);
        right = glm::vec3(-blockSize, 0, 0);
        up = glm::vec3(0, blockSize, 0);
    }

    // Compute the other three corners
    glm::vec3 bottomLeft = startCorner;
    glm::vec3 bottomRight = startCorner + right;
    glm::vec3 topLeft = startCorner + up;
    glm::vec3 topRight = startCorner + up + right;

    /* Debug
    std::cout << "Initial start corner: (" << startCorner.x << ", " << startCorner.y << ", " << startCorner.z << ")\n";

    // Debug: Output the direction vectors
    std::cout << "Right vector: (" << right.x << ", " << right.y << ", " << right.z << ")\n";
    std::cout << "Up vector: (" << up.x << ", " << up.y << ", " << up.z << ")\n";

    // Debug: Output the calculated corners for this face
    std::cout << "Adding Face with normal (" << normal.x << ", " << normal.y << ", " << normal.z << ")\n";
    std::cout << "Start corner: (" << startCorner.x << ", " << startCorner.y << ", " << startCorner.z << ")\n";
    std::cout << "Bottom left: (" << bottomLeft.x << ", " << bottomLeft.y << ", " << bottomLeft.z << ")\n";
    std::cout << "Bottom right: (" << bottomRight.x << ", " << bottomRight.y << ", " << bottomRight.z << ")\n";
    std::cout << "Top left: (" << topLeft.x << ", " << topLeft.y << ", " << topLeft.z << ")\n";
    std::cout << "Top right: (" << topRight.x << ", " << topRight.y << ", " << topRight.z << ")\n";

    // Add two triangles for the face ensuring the correct winding order
    // Debug: Output the order of vertices being added
    std::cout << "Adding vertices in order: bottomLeft, topLeft, topRight, bottomLeft, topRight, bottomRight\n";

    */

    // Add vertices for two triangles
    vertices.insert(vertices.end(), { bottomLeft.x, bottomLeft.y, bottomLeft.z,
                                      topLeft.x, topLeft.y, topLeft.z,
                                      topRight.x, topRight.y, topRight.z,
                                      bottomLeft.x, bottomLeft.y, bottomLeft.z,
                                      topRight.x, topRight.y, topRight.z,
                                      bottomRight.x, bottomRight.y, bottomRight.z });

    // Add normals for the vertices
    for (int i = 0; i < 6; ++i) {
        normals.insert(normals.end(), { normal.x, normal.y, normal.z });
    }
}


