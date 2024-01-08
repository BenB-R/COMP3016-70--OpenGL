#include "../headers/CaveGenerator.h"
#include <cmath>
#include <glm/gtc/noise.hpp> // For Perlin noise
#include <iostream>


// Constructor for the CaveGenerator class. Initializes the cave with specified dimensions and threshold
// for determining solid blocks based on Perlin noise.
// Parameters:
//   - depth: Depth of the cave (z-axis).
//   - width: Width of the cave (x-axis).
//   - height: Height of the cave (y-axis).
//   - threshold: Noise threshold for determining solid blocks.
CaveGenerator::CaveGenerator(int depth, int width, int height, float threshold)
    : depth(depth), width(width), height(height), threshold(threshold) {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    std::vector<glm::vec3> crystalPositions;

    // Pre-calculate noise values
    noiseValues.resize(depth, std::vector<std::vector<float>>(height, std::vector<float>(width)));
    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                noiseValues[z][y][x] = perlinNoise(x, y, z);
            }
        }
    }
    // Generate and apply Perlin worm
    carveCorridor(20, 40, 20, 10, 8, 40);
}

// Destructor for the CaveGenerator class. Cleans up OpenGL resources by deleting the VAO and VBO.
CaveGenerator::~CaveGenerator() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

// Generates the cave geometry by populating vertex data based on Perlin noise and determining
// which blocks are solid. It also sets up the VAO and VBO with the generated vertex data.
void CaveGenerator::generateCave() {
    std::vector<Vertex> vertexData;
    vertices.clear();
    normals.clear();

    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (isSolid(x, y, z)) {
                    // Check each face for a neighboring block and add face if no neighbor exists
                    if (!hasNeighbour(x, y, z, glm::vec3(1.0f, 0.0f, 0.0f))) {
                        addFace(vertexData, x, y, z, glm::vec3(1.0f, 0.0f, 0.0f)); // Right face
                    }
                    if (!hasNeighbour(x, y, z, glm::vec3(-1.0f, 0.0f, 0.0f))) {
                        addFace(vertexData, x, y, z, glm::vec3(-1.0f, 0.0f, 0.0f)); // Left face
                    }
                    if (!hasNeighbour(x, y, z, glm::vec3(0.0f, 1.0f, 0.0f))) {
                        addFace(vertexData, x, y, z, glm::vec3(0.0f, 1.0f, 0.0f)); // Top face
                    }
                    if (!hasNeighbour(x, y, z, glm::vec3(0.0f, -1.0f, 0.0f))) {
                        addFace(vertexData, x, y, z, glm::vec3(0.0f, -1.0f, 0.0f)); // Bottom face
                    }
                    if (!hasNeighbour(x, y, z, glm::vec3(0.0f, 0.0f, 1.0f))) {
                        addFace(vertexData, x, y, z, glm::vec3(0.0f, 0.0f, 1.0f)); // Front face
                    }
                    if (!hasNeighbour(x, y, z, glm::vec3(0.0f, 0.0f, -1.0f))) {
                        addFace(vertexData, x, y, z, glm::vec3(0.0f, 0.0f, -1.0f)); // Back face
                    }
                }
            }
        }
    }

#pragma region VAOs & VBOs
    // Update VAO and VBO for vertices, normals, and texture coordinates
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(Vertex), vertexData.data(), GL_STATIC_DRAW);

    // After buffering vertexData to the GPU
    vertexCount = vertexData.size(); // Store the number of vertices

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Vertex normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // Texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    // Unbind buffer and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
#pragma endregion
}

// Generates crystal formations within the cave by randomly placing crystals at certain positions
// based on a probability check.
void CaveGenerator::generateCrystals() {
    srand(static_cast<unsigned int>(time(0))); // Seed the random number generator

    float crystalSpawnProbability = 0.01f; // 5% chance to spawn a crystal

    for (int z = 0; z < depth; ++z) {
        for (int y = 1; y < height; ++y) { // Start from y = 1 to ensure there's a block below
            for (int x = 0; x < width; ++x) {
                // Check if the current position is an open space and has a solid block below
                if (isSolid(x, y - 1, z) && !isSolid(x, y, z)) {
                    // Generate a random number between 0 and 1
                    float randomValue = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
                    // Check if the random value is less than the probability
                    if (randomValue < crystalSpawnProbability) {
                        crystalPositions.push_back(glm::vec3(x, y, z));
                    }
                }
            }
        }
    }
}

// Renders the cave geometry by binding the VAO and drawing the vertices as triangles.
void CaveGenerator::render() {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

// Generates Perlin noise value for a given block position in the cave.
// This noise is used to determine the solidity of blocks.
// Parameters:
//   - x, y, z: The x, y, z coordinates of the block in the cave.
float CaveGenerator::perlinNoise(int x, int y, int z) {
    // Noise parameters
    float scaleX = 0.05f; // Scale for x-axis
    float scaleY = 0.05f; // Scale for y-axis
    float scaleZ = 0.05f; // Scale for z-axis
    float noise = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float persistence = 0.5f;
    int octaves = 3;

    // Modify parameters for the new biome below the biome change level
    if (y < biomeChangeYLevel) {
        scaleX = 0.1f; // Larger scale for larger features
        scaleY = 0.1f;
        persistence = 0.7f; // Change in persistence for variation
    }

    // Generate noise using Perlin noise function
    for (int i = 0; i < octaves; i++) {
        glm::vec3 pos = glm::vec3(x * scaleX * frequency, y * scaleY * frequency, z * scaleZ * frequency);
        noise += amplitude * glm::perlin(pos);
        amplitude *= persistence;
        frequency *= 2.0f;
    }

    return noise;
}

// Determines if a given block at position (x, y, z) has a neighboring solid block in the specified direction.
// Parameters:
//   - x, y, z: The x, y, z coordinates of the block in the cave.
//   - direction: A glm::vec3 vector indicating the direction to check for a neighbor.
bool CaveGenerator::hasNeighbour(int x, int y, int z, glm::vec3 direction) {
    // Check for boundaries
    if (x + direction.x < 0 || x + direction.x >= width ||
        y + direction.y < 0 || y + direction.y >= height ||
        z + direction.z < 0 || z + direction.z >= depth) {
        return false;
    }
    return isSolid(x + direction.x, y + direction.y, z + direction.z);
}

// Checks if a block at a given position is solid based on the noise value and threshold.
// Parameters:
//   - x, y, z: The x, y, z coordinates of the block in the cave.
bool CaveGenerator::isSolid(int x, int y, int z) {
    return noiseValues[z][y][x] < threshold;
}

// Adds the vertices for a block face to the vertex data if the block at position (x, y, z) is solid
// and doesn't have a neighboring block in the direction of the normal.
// Parameters:
//   - vertexData: A reference to the vector of Vertex structs where the vertex data will be added.
//   - x, y, z: The x, y, z coordinates of the block in the cave.
//   - normal: A glm::vec3 vector indicating the normal direction of the face to be added.
void CaveGenerator::addFace(std::vector<Vertex>& vertexData, int x, int y, int z, glm::vec3 normal) {
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

    // Texture coordinates for each vertex of the face
    std::vector<glm::vec2> texCoords = {
        glm::vec2(0.0f, 0.0f), // Bottom left
        glm::vec2(0.0f, 1.0f), // Top left
        glm::vec2(1.0f, 1.0f), // Top right
        glm::vec2(0.0f, 0.0f), // Bottom left
        glm::vec2(1.0f, 1.0f), // Top right
        glm::vec2(1.0f, 0.0f)  // Bottom right
    };

    for (int i = 0; i < 6; ++i) {
        Vertex vertex;
        // Assign positions based on the order of vertices for two triangles
        glm::vec3 positions[6] = { bottomLeft, topLeft, topRight, bottomLeft, topRight, bottomRight };
        vertex.position = positions[i];
        vertex.normal = normal;
        vertex.texCoords = texCoords[i];
        vertexData.push_back(vertex);
    }
}

// Carves out a corridor in the cave by marking blocks as non-solid within the specified range.
// Parameters:
//   - startX, startY, startZ: Starting x, y, z coordinates of the corridor.
//   - corridorWidth, corridorHeight, corridorDepth: Width, height, and depth of the corridor to carve out.
void CaveGenerator::carveCorridor(int startX, int startY, int startZ, int corridorWidth, int corridorHeight, int corridorDepth) {
    int endX = std::min(width, startX + corridorWidth);
    int endY = std::min(height, startY + corridorHeight);
    int endZ = std::min(depth, startZ + corridorDepth);

    for (int x = startX; x < endX; ++x) {
        for (int y = startY; y < endY; ++y) {
            for (int z = startZ; z < endZ; ++z) {
                // Mark the block as non-solid (or remove it) to create the corridor
                noiseValues[z][y][x] = 1.0f; // Assuming higher noise value means non-solid
            }
        }
    }
}