#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <iostream>
#include <string>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <algorithm>
#include <cmath>
#include "../../stb_image/stb_image.h"
#include "../../Shader/Shader.h"
#include "../Mesh/Mesh.h"

unsigned int TextureFromFile(const char *path, const std::string &directory);

struct MinMaxVertices
{
    glm::vec3 max = glm::vec3(0.0f);
    glm::vec3 min = glm::vec3(0.0f);
};

class Model
{
private:
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    std::string directory;
    MinMaxVertices minmaxVertices;

    void
    loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    void processVertexMinmaxs(const glm::vec3 vertex);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType aiTexType, TextureType texType, unsigned int materialIndex);

public:
    Model(char *path);
    void Draw(Shader &shader);
    void DrawWithHighlight(Shader &shader, Shader &highlightShader, int id = 1);
    glm::vec3 getModelCenter();
    float getModelHeight();
};

#endif