#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

const float CAM_YAW = -90.0f;
const float CAM_PITCH = 0.0f;
const float CAM_SPEED = 2.5f;
const float CAM_SENSITIVITY = 0.1f;
const float CAM_ZOOM = 45.0f;

class Camera
{
private:
    void updateCameraVectors();

public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    glm::vec3 *target;
    glm::vec3 positionFromTarget;
    float yaw;
    float pitch;
    float movementSpeed;
    float mouseSensitivity;
    float zoom;
    bool lookAtTarget = false;
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = CAM_YAW, float pitch = CAM_PITCH);
    glm::mat4 GetViewMatrix();
    void followTarget();
    void setTarget(glm::vec3 *target, glm::vec3 positionFromTarget);
    void removeTarget();
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void ProcessMouseMovement(float xoffset, float yoffset, glm::vec3 target, GLboolean constrainPitch = true);
    // void ProcessMouseScroll(float yoffset);
};

#endif