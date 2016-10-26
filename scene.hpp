#pragma once

#include <GL/glew.h>

#include <model.hpp>
#include <camera.hpp>
#include <renderer.hpp>

class Scene {
public:
  Scene(ScreenRender* screen, FrameBufferRender* fb_render);

  void render();

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
  FrameBufferRender* fb_render;

  Model flag;
  
  GLfloat deltaTime;
  GLfloat lastFrame;

  double lastTime;
  double lastUpdate;
  float fps;
  float gamma;
  bool next;
};