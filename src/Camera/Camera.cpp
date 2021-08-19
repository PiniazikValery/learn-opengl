#include "Camera.h"

void Camera::updateCameraVectors()
{
    if (lookAtTarget)
    {
        followTarget();
    }
    else
    {
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        this->front = glm::normalize(front);
        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }
}

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(CAM_SPEED), mouseSensitivity(CAM_SENSITIVITY), zoom(CAM_ZOOM), target(nullptr)
{
    worldUp = up;
    this->position = position;
    this->yaw = yaw;
    this->pitch = pitch;
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    if (lookAtTarget)
    {
        return glm::lookAt(position, *target, worldUp);
    }
    else
    {
        return glm::lookAt(position, position + front, up);
    }
}

void Camera::followTarget()
{
    if (lookAtTarget)
    {
        float camRange = glm::length(glm::vec3(0) - positionFromTarget);
        positionFromTarget.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)) * -camRange;
        positionFromTarget.y = sin(glm::radians(pitch)) * -camRange;
        positionFromTarget.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch)) * -camRange;
        position = *target + positionFromTarget;
        front = glm::normalize(*target - position);
        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }
}

void Camera::setTarget(glm::vec3 *target, glm::vec3 positionFromTarget)
{
    lookAtTarget = true;
    this->target = target;
    this->positionFromTarget = -positionFromTarget;
    updateCameraVectors();
}

void Camera::removeTarget()
{
    lookAtTarget = false;
    this->target = nullptr;
    this->positionFromTarget = glm::vec3(0);
    updateCameraVectors();
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = movementSpeed * deltaTime;
    if (direction == FORWARD)
        position += front * velocity;
    if (direction == BACKWARD)
        position -= front * velocity;
    if (direction == LEFT)
        position -= right * velocity;
    if (direction == RIGHT)
        position += right * velocity;
    if (direction == UP)
        position += worldUp * velocity;
    if (direction == DOWN)
        position -= worldUp * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (constrainPitch)
    {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    updateCameraVectors();
}