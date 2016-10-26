#pragma once
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <shader.hpp>
#include <model.hpp>
#include <camera.hpp>



class SceneRender {
public:
  std::vector<Drawable*> models;
protected:
  SceneRender();
};

class ScreenRender : public SceneRender {

public:
  ScreenRender(GLFWwindow* window);

  void render(IsoCamera& perspective);

  GLFWwindow* window;
  int width, height;

  Shader shader;
  void setupLights(IsoCamera& perspective);
};

class FrameBufferRender : public SceneRender {

public:
  FrameBufferRender(int width, int height);

  void render(IsoCamera& perspective);

  Texture getTexture();

private:
  GLuint FramebufferName;
  Texture renderedTexture;
  int width, height;

  GLuint pbos[2];
  GLuint active_pbo;
  OrthoCamera camera;


  Shader shader;
  void setupLights(IsoCamera& perspective);

  std::vector<uint8_t> frameBuffer;
};

class PatternRender {
public:
  PatternRender(int width, int height);

  void render(Shader& pattern);

  Texture getTexture();
private:
  GLuint VertexArrayID;
  GLuint vertexbuffer;

  GLuint FramebufferName;
  Texture renderedTexture;
  int width, height;

};

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