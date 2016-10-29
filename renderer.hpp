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
  FrameBufferRender(const Texture& renderTo);

  void render(const IsoCamera& perspective, uint8_t* data, size_t size);

  Texture getTexture();

private:
  GLuint FramebufferName;
  Texture renderedTexture;
  int width, height;

  GLuint pbos[2];
  GLuint active_pbo;
  OrthoCamera camera;


  Shader shader;
  void setupLights(const IsoCamera& perspective);
};

class PatternRender {
public:
  PatternRender(const Texture& renderTo);

  void render(const Shader& pattern);

  const Texture& getTexture();
private:
  GLuint VertexArrayID;
  GLuint vertexbuffer;

  GLuint FramebufferName;
  Texture renderedTexture;
  int width, height;

};
