#pragma once
#include <vector>

#include <opengl.h>
#include <glm/glm.hpp>

#include <shader.hpp>
#include <texture.hpp>
#include <chrono>
#include <vector>


class PatternRender {
public:
  PatternRender(glm::vec2 canvasSize);

  void render(const Shader& pattern);

  const Texture& getTexture();

  std::vector<uint8_t> colors;
  int width, height;
private:
  GLuint VertexArrayID;
  GLuint vertexbuffer;

  GLuint FramebufferName;
  Texture renderedTexture;

  GLuint pbos[2];
  GLuint active_pbo;

  std::chrono::time_point<std::chrono::high_resolution_clock> start;

};
