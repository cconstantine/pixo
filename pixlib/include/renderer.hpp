#pragma once
#include <vector>

#include <opengl.h>

#include <shader.hpp>
#include <cube.hpp>
#include <camera.hpp>
#include <chrono>



class SceneRender {
public:
  std::vector<Drawable*> models;
protected:
  SceneRender();
};

class ScreenRender : public SceneRender {

public:
  ScreenRender();

  void render(const IsoCamera& perspective, int width, int height);

  Shader shader;
  void setupLights(const IsoCamera& perspective);
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
  PatternRender(glm::vec2 canvasSize);

  void render(const Shader& pattern);

  const Texture& getTexture();
private:
  GLuint VertexArrayID;
  GLuint vertexbuffer;

  GLuint FramebufferName;
  Texture renderedTexture;
  int width, height;

  std::chrono::time_point<std::chrono::high_resolution_clock> start;

};
