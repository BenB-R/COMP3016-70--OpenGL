#ifndef CAVEGENERATOR_H
#define CAVEGENERATOR_H

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "crystal.h"

class CaveGenerator {
public:
    CaveGenerator(int depth, int width, int height, float threshold);
    ~CaveGenerator();

    void generateCave();
    void render();

    std::vector<std::vector<std::vector<float>>> noiseValues;
    std::vector<Crystal> crystals;
    const std::vector<glm::vec3>& getCrystalPositions() const { return crystalPositions; };
    void generateCrystals();

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
    };

private:
    int depth, width, height;
    float threshold;
    unsigned int vertexCount;
    const int biomeChangeYLevel = 20;
    std::vector<GLfloat> vertices;
    std::vector<float> normals;
    std::vector<glm::vec3> crystalPositions; // Member variable to store crystal positions
    GLuint vao, vbo;

    bool hasNeighbour(int x, int y, int z, glm::vec3 direction);
    float perlinNoise(int x, int y, int z);
    bool isSolid(int x, int y, int z);
    void addFace(std::vector<Vertex>& vertexData, int x, int y, int z, glm::vec3 normal);
    void generatePerlinWorm(int startX, int startY, int startZ, int length, float thickness);
    void carveTunnel(float x, float y, float z, float radius);
    void carveCorridor(int startX, int startY, int startZ, int corridorWidth, int corridorHeight, int corridorDepth);
    void updateLighting(int y);
};

#endif // CAVEGENERATOR_H
