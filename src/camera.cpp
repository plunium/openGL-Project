#include "camera.hpp"

// Constructeur avec des vecteurs
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : _front(CAMERA_START_FRONT), _movementSpeed(CAMERA_SPEED), _mouseSensitivity(MOUSE_SENSITIVITY), _zoom(START_FOV)
{
    _position = position;
    _worldUp = up;
    _yaw = yaw;
    _pitch = pitch;
    updateCameraVectors();
}

// Constructeur avec des scalaires (valeurs par défaut)
Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : _front(CAMERA_START_FRONT), _movementSpeed(CAMERA_SPEED), _mouseSensitivity(MOUSE_SENSITIVITY), _zoom(START_FOV)
{
    _position = glm::vec3(posX, posY, posZ);
    _worldUp = glm::vec3(upX, upY, upZ);
    _yaw = yaw;
    _pitch = pitch;
    updateCameraVectors();
}

// Renvoie la matrice de vue calculée à l'aide des angles d'Euler et de la matrice LookAt
glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(_position, _position + _front, _up);
}

// Traite l'entrée reçue de tout système d'entrée de type clavier. Accepte le paramètre d'entrée sous la forme d'une énumération définie par la caméra (pour l'abstraire des systèmes de fenêtrage)
void Camera::processKeyboard(CameraMovement direction, float deltaTime)
{
    float velocity = _movementSpeed * deltaTime;
    if (direction == FORWARD)
        _position += _front * velocity;
    if (direction == BACKWARD)
        _position -= _front * velocity;
    if (direction == LEFT)
        _position -= _right * velocity;
    if (direction == RIGHT)
        _position += _right * velocity;
}

// Traite l'entrée reçue d'un système d'entrée de type souris. Attend la valeur de décalage dans les deux directions x et y.
void Camera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= _mouseSensitivity;
    yoffset *= _mouseSensitivity;

    _yaw += xoffset;
    _pitch += yoffset;

    // Lorsque le pitch est hors limites, l'écran ne se retourne pas.
    if (constrainPitch)
    {
        if (_pitch > 89.0f)
            _pitch = 89.0f;
        if (_pitch < -89.0f)
            _pitch = -89.0f;
    }

    // Mettre à jour les vecteurs Front, Right et Up à l'aide des angles d'Euler mis à jour
    updateCameraVectors();
}

// Traite l'entrée reçue d'un événement de molette de défilement de souris. N'exige d'entrée que sur l'axe de la roue verticale
void Camera::processMouseScroll(float yoffset)
{
    _zoom -= (float)yoffset;
    if (_zoom < 1.0f)
        _zoom = 1.0f;
    if (_zoom > 45.0f)
        _zoom = 45.0f;
}

// Calcule le vecteur front de la caméra à partir des angles d'Euler (mis à jour) de la caméra
void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    front.y = sin(glm::radians(_pitch));
    front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    _front = glm::normalize(front);
    // Calculer le vecteur Right et Up à partir du vecteur Front et du vecteur WorldUp
    _right = glm::normalize(glm::cross(_front, _worldUp)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    _up = glm::normalize(glm::cross(_right, _front));
}