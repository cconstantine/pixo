#include <pixlib/camera.hpp>
#include <glm/ext.hpp>

// Std. Includes
#include <vector>

namespace Pixlib {

  // Constructor with vectors
  IsoCamera::IsoCamera(GLfloat yaw, GLfloat pitch) :
          WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
          Yaw(yaw),
          Pitch(pitch),
          MouseSensitivity(SENSITIVTY),
          Zoom(ZOOM)
  {
    this->updateCameraVectors();
  }


  // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
  glm::mat4 IsoCamera::GetViewMatrix() const
  {
      return glm::lookAt(this->Position, this->Front, glm::vec3(0.0f, 1.0f, 0.0f));
  }

  glm::mat4 IsoCamera::GetProjectionMatrix(int width, int height) const
  {
    return glm::perspective(45.f, (float)width/(float)height, 0.1f, 1000.0f);
  }

  // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
  void IsoCamera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset)
  {
    xoffset *= this->MouseSensitivity;
    yoffset *= this->MouseSensitivity;

    this->Yaw   += xoffset;
    this->Pitch += yoffset;

    if (this->Pitch > 89.0f)
      this->Pitch = 89.0f;
    if (this->Pitch < -89.0f)
      this->Pitch = -89.0f;

    // Update Front, Right and Up Vectors using the updated Eular angles
    this->updateCameraVectors();
  }

  // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
  void IsoCamera::ProcessMouseScroll(GLfloat yoffset)
  {
    this->Zoom += -yoffset * 0.15f;
    if (this->Zoom < 0.15f) {
      this->Zoom = 0.15f;
    }

    this->updateCameraVectors();
  }

  void IsoCamera::moveTowards(const IsoCamera& camera, float timeScale)
  {
    Yaw = Yaw + (camera.Yaw - Yaw) * timeScale;
    Pitch = Pitch + (camera.Pitch - Pitch) * timeScale;
    Zoom = Zoom + (camera.Zoom - Zoom) * timeScale;

    updateCameraVectors();
  }
  // Calculates the front vector from the Camera's (updated) Eular Angles
  void IsoCamera::updateCameraVectors()
  {
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
    front.y = sin(glm::radians(this->Pitch));
    front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
    this->Position = front*Zoom;
    this->Front = glm::vec3(0.0f);

    // Also re-calculate the Right and Up vector
    this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    this->Up    = glm::normalize(glm::cross(this->Right, this->Front));  
  }




  float IsoCamera::getZoom() const
  {
    float zoom = 0.0f;

    glm::vec3 edge(scope.x, scope.y, scope.z);

    float position_distance = glm::length(Position);
    float edge_distance = glm::length(edge);

    glm::vec2 a = glm::normalize(glm::vec2(position_distance, edge_distance ));
    glm::vec2 b = glm::vec2(1, 0.0f);


    zoom = glm::angle(a, b)*2.5;

    return zoom;
  }

  void IsoCamera::rotate(const float rads)
  {
    Yaw += rads;
    updateCameraVectors();
  }
}