#include "Model.h"

void Model::Draw(Shader &shader)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i].Draw(shader);
    }
}

void Model::DrawWithHighlight(Shader &shader, Shader &highlightShader, int ref)
{
    glStencilFunc(GL_ALWAYS, ref, 0xFF);
    glStencilMask(0xFF);

    shader.use();
    Draw(shader);

    glStencilFunc(GL_GREATER, ref, 0xFF);
    glStencilMask(0x00);

    highlightShader.use();
    Draw(highlightShader);

    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, ref, 0xFF);
}

void Model::loadModel(std::string path)
{
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "Assimp error: " << import.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector;

        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        processVertexMinmaxs(vector);

        if (mesh->mNormals)
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        else
        {
            vertex.Normal = glm::vec3(0.0f);
        }

        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f);
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, DIFFUSE, mesh->mMaterialIndex);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, SPECULAR, mesh->mMaterialIndex);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        std::vector<Texture> emissionMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, EMISSION, mesh->mMaterialIndex);
        textures.insert(textures.end(), emissionMaps.begin(), emissionMaps.end());
    }

    return Mesh(vertices, indices, textures, mesh->mMaterialIndex);
}

void Model::processVertexMinmaxs(const glm::vec3 vertex)
{
    minmaxVertices.max.x = std::max(minmaxVertices.max.x, vertex.x);
    minmaxVertices.max.y = std::max(minmaxVertices.max.y, vertex.y);
    minmaxVertices.max.z = std::max(minmaxVertices.max.z, vertex.z);

    minmaxVertices.min.x = std::min(minmaxVertices.min.x, vertex.x);
    minmaxVertices.min.y = std::min(minmaxVertices.min.y, vertex.y);
    minmaxVertices.min.z = std::min(minmaxVertices.min.z, vertex.z);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType aiTexType, TextureType texType, unsigned int materialIndex)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(aiTexType); i++)
    {
        aiString str;
        mat->GetTexture(aiTexType, i, &str);
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip)
        {
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), directory);
            texture.type = texType;
            texture.path = str.C_Str();
            texture.materialIndex = materialIndex;
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}

unsigned int TextureFromFile(const char *path, const std::string &directory)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load texture at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

Model::Model(char *path)
{
    loadModel(path);
}

glm::vec3 Model::getModelCenter()
{
    glm::vec3 result;

    result.x = (minmaxVertices.max.x - minmaxVertices.min.x) / 2;
    result.y = (minmaxVertices.max.y - minmaxVertices.min.y) / 2;
    result.z = (minmaxVertices.max.z - minmaxVertices.min.z) / 2;

    return result;
}

float Model::getModelHeight()
{
    return std::abs(minmaxVertices.max.y - minmaxVertices.min.y);
}