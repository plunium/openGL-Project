#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "constants.hpp"

// Une classe de caméra abstraite qui traite les entrées et calcule les angles d'Euler, les vecteurs et les matrices correspondants pour une utilisation dans OpenGL
class Camera
{
public:
    // Constructeur avec des vecteurs
    Camera(glm::vec3 position = CAMERA_START_POSITION, glm::vec3 up = CAMERA_START_UP, float yaw = START_CAMERA_YAW, float pitch = 0.0);
    // Constructeur avec des scalaires (valeurs par défaut)
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    // Renvoie la matrice de vue calculée à l'aide des angles d'Euler et de la matrice LookAt
    glm::mat4 getViewMatrix();

    float getZoom() { return _zoom; }

    glm::vec3 getPosition() { return _position; }

    glm::vec3 getFront() { return _front; }

    // Traite l'entrée reçue de tout système d'entrée de type clavier. Accepte le paramètre d'entrée sous la forme d'une énumération définie par la caméra.
    void processKeyboard(CameraMovement direction, float deltaTime);

    // Traite l'entrée reçue d'un système d'entrée de type souris. Attend la valeur de décalage dans les deux directions x et y.
    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    // Traite l'entrée reçue d'un événement de molette de défilement de souris. N'exige d'entrée que sur l'axe de la roue verticale
    void processMouseScroll(float yoffset);

private:
    // Position de la caméra dans le monde
    glm::vec3 _position;
    // Vecteur de direction (regard) de la caméra
    glm::vec3 _front;
    // Vecteur haut de la caméra
    glm::vec3 _up;
    // Vecteur droite de la caméra
    glm::vec3 _right;
    // Vecteur haut du monde
    glm::vec3 _worldUp;
    // Les angles d'Euler (en degrés, non en radians).
    float _yaw;
    float _pitch;
    // Options de la caméra
    float _movementSpeed;
    float _mouseSensitivity;
    float _zoom;

    // Calcule le vecteur front de la caméra à partir des angles d'Euler (mis à jour) de la caméra
    void updateCameraVectors();
};

#endif