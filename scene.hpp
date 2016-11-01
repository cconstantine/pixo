#pragma once

#include <GL/glew.h>

#include <model.hpp>
#include <camera.hpp>
#include <renderer.hpp>
#include <led_cluster.hpp>

class Scene {
public:
  Scene(ScreenRender* screen, LedCluster* leds);

  void render(const Shader& pattern);

  void Do_Movement();

  float getGamma();
  bool nextPattern();

  void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
  void mouse_callback(GLFWwindow* window, double xpos, double ypos);
  void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
  void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

  float getFps();
private:
  IsoCamera perspective;
  IsoCamera viewed_from;
  ScreenRender* screen;
  
  LedCluster* leds;

  Model flag;
  
  GLfloat deltaTime;
  GLfloat lastFrame;

  double lastTime;
  double lastUpdate;
  float fps;
  unsigned int frames;
  float gamma;
  bool next;
};