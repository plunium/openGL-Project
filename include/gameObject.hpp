#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <memory>

#include "model.hpp"
#include "shader.hpp"

using namespace std;

class GameObject
{
public:
    // Constructeur principal
    GameObject(string name, string path, bool flipTextureVertically, Shader &shader, glm::mat4 modelMatrix, std::vector<std::unique_ptr<GameObject>> &gameObjects);

    // Constructeur sans nom (utilise le chemin comme nom)
    GameObject(string path, bool flipTextureVertically, Shader &shader, glm::mat4 modelMatrix, std::vector<std::unique_ptr<GameObject>> &gameObjects)
        : GameObject("", path, flipTextureVertically, shader, modelMatrix, gameObjects) {}

    // Constructeur sans transformation (utilise une matrice identité)
    GameObject(string name, string path, bool flipTextureVertically, Shader &shader, std::vector<std::unique_ptr<GameObject>> &gameObjects)
        : GameObject(name, path, flipTextureVertically, shader, glm::mat4(1.0f), gameObjects) {}

    // Constructeur sans nom et sans transformation
    GameObject(string path, bool flipTextureVertically, Shader &shader, std::vector<std::unique_ptr<GameObject>> &gameObjects)
        : GameObject("", path, flipTextureVertically, shader, glm::mat4(1.0f), gameObjects) {}

    ~GameObject() { graphicModel.CleanUp(); }

    void Draw();

    string getName() { return name; }

    glm::mat4 getModelMatrix() { return modelMatrix; }
    void modelMatrixTranslate(glm::vec3 translation) { modelMatrix = glm::translate(modelMatrix, translation); }
    void modelMatrixRotate(float angle, glm::vec3 axis) { modelMatrix = glm::rotate(modelMatrix, angle, axis); }
    void modelMatrixScale(glm::vec3 scale) { modelMatrix = glm::scale(modelMatrix, scale); }

private:
    string name;
    Model graphicModel;
    Shader &shader;
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    std::vector<std::unique_ptr<GameObject>> &gameObjects;
};

#endif