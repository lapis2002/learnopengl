# include <Camera.h>

// -------------------------------------------------------------------------------------------------------------------------------------
// returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(Position, Position + Front, Up);
}

// -------------------------------------------------------------------------------------------------------------------------------------
// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::processKeyboard(int direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    switch (direction) {
        case (CAMERA_MOVEMENT::FORWARD):
            Position += Front * velocity;
            break;

        case (CAMERA_MOVEMENT::BACKWARD):
            Position -= Front * velocity;
            break;
        
        case (CAMERA_MOVEMENT::RIGHT):
            Position += Right * velocity;
            break;

        case (CAMERA_MOVEMENT::LEFT):
            Position -= Right * velocity;
            break;
    }
}

// -------------------------------------------------------------------------------------------------------------------------------------
// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch) {
    xOffset *= MouseSensitivity;
    yOffset *= MouseSensitivity;

    Yaw += xOffset;
    Pitch += yOffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}
// -------------------------------------------------------------------------------------------------------------------------------------
void Camera::processMouseScroll(float yOffset) {
    Zoom -= static_cast<float>(yOffset);
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}
// -------------------------------------------------------------------------------------------------------------------------------------
// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}