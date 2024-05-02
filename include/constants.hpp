#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "color.hpp"

constexpr float WINDOW_WIDTH = 800.0f;
constexpr float WINDOW_HEIGHT = 600.0f;

constexpr glm::vec3 CAMERA_START_POSITION(0.0f, 0.0f, 3.0f);
constexpr glm::vec3 CAMERA_START_FRONT(0.0f, 0.0f, -1.0f);
constexpr glm::vec3 CAMERA_START_UP(0.0f, 1.0f, 0.0f);

constexpr float START_CAMERA_YAW = -90.0f;
constexpr float START_CAMERA_PITCH = 0.0f;

constexpr float START_FOV = 45.0f;

constexpr float CAMERA_SPEED = 5.0f;

constexpr float MOUSE_SENSITIVITY = 0.1f;

constexpr const char * WINDOW_NAME = "My Game";

constexpr float PITCH_LIMIT = 89.0f;

constexpr const char * OBJECT_VERTEX_SHADER_PATH = "shaders/objectShader.vs";
constexpr const char * OBJECT_FRAGMENT_SHADER_PATH = "shaders/objectShader.fs";

constexpr const char * LIGHT_VERTEX_SHADER_PATH = "shaders/lightShader.vs";
constexpr const char * LIGHT_FRAGMENT_SHADER_PATH = "shaders/lightShader.fs";

constexpr const char * TEXTURE_1_PATH = "resources/textures/wall.jpg";
constexpr const char * TEXTURE_2_PATH = "resources/textures/smiley.png";

constexpr Color CLEAR_COLOR(0.1f, 0.1f, 0.1f, 1.0f);

constexpr float NEAR_CLIP_PLANE_DISTANCE = 0.1f;
constexpr float FAR_CLIP_PLANE_DISTANCE = 100.0f;

constexpr float LIGHT_SPEED = 2.5f;

constexpr glm::vec3 LIGHT_SOURCE_POSITION(1.2f, 1.0f, -2.0f);

enum CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

#endif