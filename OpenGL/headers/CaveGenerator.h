#ifndef CAVEGENERATOR_H
#define CAVEGENERATOR_H

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

class CaveGenerator {
public:
    CaveGenerator(int depth, int width, int height, float threshold);
    ~CaveGenerator();

    void generateCave();
    void render();

private:
    int depth, width, height;
    float threshold;
    std::vector<GLfloat> vertices;
    std::vector<float> normals;
    GLuint vao, vbo;

    bool hasNeighbour(int x, int y, int z, glm::vec3 direction);
    float perlinNoise(int x, int y, int z);
    bool isSolid(int x, int y, int z);
    void addFace(int x, int y, int z, glm::vec3 normal);
};

#endif // CAVEGENERATOR_H
