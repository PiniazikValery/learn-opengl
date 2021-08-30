#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <iostream>
#include <string>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "../../stb_image/stb_image.h"
#include "../../Shader/Shader.h"
#include "../Mesh/Mesh.h"

unsigned int TextureFromFile(const char *path, const std::string &directory);

class Model
{
private:
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    std::string directory;

    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType aiTexType, TextureType texType);

public:
    Model(char *path);
    void Draw(Shader &shader);
};

#endif