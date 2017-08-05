#pragma once
#include <vector>

#include <opengl.h>
#include <glm/glm.hpp>

#include <pattern.hpp>
#include <texture.hpp>
#include <chrono>


class PatternRender {
public:
  PatternRender(glm::vec2 canvasSize);

  void render(const Pattern& pattern);

  const Texture& getTexture();
private:
  GLuint VertexArrayID;
  GLuint vertexbuffer;

  GLuint FramebufferName;
  Texture renderedTexture;
  int width, height;


};
