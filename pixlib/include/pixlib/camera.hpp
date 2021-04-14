#pragma once

// Std. Includes
#include <vector>

// GL Includes
#include <opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <pixlib/data_model.hpp>


namespace Pixlib {
  // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods

  // Default camera values
  const GLfloat YAW        = 108.0f;
  const GLfloat PITCH      =  12.0f;
  const GLfloat SENSITIVTY =  0.25f;
  const GLfloat ZOOM       =  2.4f;

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

    glm::vec3 scope;
    int width, height;

    // Constructor with vectors
    IsoCamera( );

    IsoCamera(const Perspective& perspective);


    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    glm::mat4 get_view_matrix() const;
    glm::mat4 get_projection_matrix(float zoom = 45.0f) const;

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void process_mouse_movement(GLfloat xoffset, GLfloat yoffset);

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void process_mouse_scroll(GLfloat yoffset);

    void move_towards(IsoCamera& camera, float timeScale);
    void move_towards(const glm::vec3, float timeScale);

    float get_zoom(bool overscan) const;

    void rotate(const float rads);
  private:
    // Calculates the front vector from the Camera's (updated) Eular Angles
    void update_camera_vectors();
  };
}