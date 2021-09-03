#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <glad/glad.h>
#include "../../Shader/Shader.h"

enum TextureType
{
    DIFFUSE,
    SPECULAR,
    EMISSION
};

std::string getTextureTypeName(TextureType type);

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture
{
    unsigned int id;
    unsigned int materialIndex;
    TextureType type;
    std::string path;
};

class Mesh
{
private:
    unsigned int VAO, VBO, EBO;

    void setupMesh();

public:
    unsigned int materialIndex;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, unsigned int materialIndex);
    void Draw(Shader shader);
};

#endif