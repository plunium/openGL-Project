#include "gameObject.hpp"

GameObject::GameObject(string name, string path, bool flipTextureVertically, Shader &shader, glm::mat4 modelMatrix, std::vector<std::unique_ptr<GameObject>>& gameObjects) : shader(shader), graphicModel(path, flipTextureVertically), gameObjects(gameObjects), modelMatrix(modelMatrix)
{
    if (name.empty())
    {
        // On récupère juste le nom du fichier sans l'extension
        size_t lastSlash = path.find_last_of("/\\");
        size_t lastDot = path.find_last_of(".");
        this->name = path.substr(lastSlash + 1, lastDot - lastSlash - 1);
    }
    else
    {
        this->name = name;
    }

    // Vérification de l'unicité du nom et ajustement si nécessaire
    int nameSuffix = 1;               // Suffixe à ajouter au nom si nécessaire
    string originalName = this->name; // Conserve le nom original pour la vérification
    bool nameIsUnique = false;

    while (!nameIsUnique)
    {
        nameIsUnique = true; // On part du principe que le nom est unique

        // On parcourt tous les GameObjects pour vérifier l'unicité du nom
        for (const auto &gameObject : gameObjects)
        {
            if (gameObject->name == this->name)
            {
                nameIsUnique = false; // Le nom n'est pas unique, on doit le modifier
                break;                // Pas besoin de continuer la vérification
            }
        }

        if (!nameIsUnique)
        {
            // Si le nom n'est pas unique, on ajoute/incremente le suffixe et réessaye
            this->name = originalName + std::to_string(++nameSuffix);
        }
    }
}

void GameObject::Draw()
{
    shader.use();
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    graphicModel.Draw(shader);
}