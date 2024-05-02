#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader.hpp"
#include "mesh.hpp"
#include "stb_image.h"

class Model
{
public:
    Model(string path, bool flipTextureVertically)
    {
        stbi_set_flip_vertically_on_load(flipTextureVertically);
        loadModel(path);
    }
    void Draw(Shader &shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }
    void CleanUp()
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].CleanUp();
    }

private:
    // On stocke toutes les textures chargées pour vérifier par la suite si elles sont déjà chargées
    vector<Texture> textures_loaded;

    // Les meshes dont est composé le modèle
    vector<Mesh> meshes;
    // Le dossier dans lequel se trouve le modèle
    string directory;

    void loadModel(string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                         string typeName);
};

#endif