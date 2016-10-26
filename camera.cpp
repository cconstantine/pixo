#include <camera.hpp>

// Std. Includes
#include <vector>

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


glm::mat4 OrthoCamera::GetViewMatrix() {
  return glm::mat4(
     1.000000, 0.000000, 0.000000, 0.000000,
     0.000000, 1.000000, 0.000000, 0.000000,
     0.000000, 0.000000, 1.000000, 0.000000,
     0.000000, 0.000000, 0.000000, 1.000000);
}

glm::mat4 OrthoCamera::GetProjectionMatrix(int width, int height) {
  return glm::ortho(0.0f, (float)width, 0.0f, (float)height);
}


// Constructor with vectors
IsoCamera::IsoCamera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
{
  this->Position = position;
  this->WorldUp = up;
  this->Yaw = yaw;
  this->Pitch = pitch;
  this->updateCameraVectors();
}

// Constructor with scalar values
IsoCamera::IsoCamera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
{
  this->Position = glm::vec3(posX, posY, posZ);
  this->WorldUp = glm::vec3(upX, upY, upZ);
  this->Yaw = yaw;
  this->Pitch = pitch;
  this->updateCameraVectors();
}

// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
glm::mat4 IsoCamera::GetViewMatrix() const
{
    return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
}

glm::mat4 IsoCamera::GetProjectionMatrix(int width, int height) const
{
  return glm::perspective(45.f, (float)width/(float)height, 0.1f, 1000.0f);
}

// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void IsoCamera::ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
{
  GLfloat velocity = this->MovementSpeed * deltaTime;
  if (direction == FORWARD)
    this->Position += this->Front * velocity;
  if (direction == BACKWARD)
    this->Position -= this->Front * velocity;
  if (direction == LEFT)
    this->Position -= this->Right * velocity;
  if (direction == RIGHT)
    this->Position += this->Right * velocity;
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void IsoCamera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch)
{
  xoffset *= this->MouseSensitivity;
  yoffset *= this->MouseSensitivity;

  this->Yaw   += xoffset;
  this->Pitch += yoffset;

  // Make sure that when pitch is out of bounds, screen doesn't get flipped
  if (constrainPitch)
  {
    if (this->Pitch > 89.0f)
      this->Pitch = 89.0f;
    if (this->Pitch < -89.0f)
      this->Pitch = -89.0f;
  }

  // Update Front, Right and Up Vectors using the updated Eular angles
  this->updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void IsoCamera::ProcessMouseScroll(GLfloat yoffset)
{
  if (this->Zoom >= 0.0f && this->Zoom <= 90.0f)
    this->Zoom += yoffset * 0.15;
  if (this->Zoom <= 0.0f)
    this->Zoom = 0.0f;
  if (this->Zoom >= 90.0f)
    this->Zoom = 90.0f;
}

// Calculates the front vector from the Camera's (updated) Eular Angles
void IsoCamera::updateCameraVectors()
{
  // Calculate the new Front vector
  glm::vec3 front;
  front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
  front.y = sin(glm::radians(this->Pitch));
  front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
  this->Front = glm::normalize(front);
  // Also re-calculate the Right and Up vector
  this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
  this->Up    = glm::normalize(glm::cross(this->Right, this->Front));
}
