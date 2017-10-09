#pragma once

// Std. Includes
#include <vector>

// GL Includes
#include <opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace Pixlib {
  // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
  enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    TOWARDS_VIEW,
    MATCH_VIEW,
    NEXT_PATTERN,
    ORB_UP,
    ORB_DOWN,
    ORB_LEFT,
    ORB_RIGHT
  };

  // Default camera values
  const GLfloat YAW        = 108.0f;
  const GLfloat PITCH      =  12.0f;
  const GLfloat SENSITIVTY =  0.25f;
  const GLfloat ZOOM       =  11.4f;

  class OrthoCamera {
  public:
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix(int width, int height);
  };

  // An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
  class IsoCamera 
  {
  public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Eular Angles
    GLfloat Yaw;
    GLfloat Pitch;
    // Camera options
    GLfloat MouseSensitivity;
    GLfloat Zoom;

    // Constructor with vectors
    IsoCamera( GLfloat yaw = YAW, GLfloat pitch = PITCH);


    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix(int width, int height) const;

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset);

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(GLfloat yoffset);

    void moveTowards(const IsoCamera& camera, float timeScale);

    void rotate(const float degs);
  private:
    // Calculates the front vector from the Camera's (updated) Eular Angles
    void updateCameraVectors();
  };
}