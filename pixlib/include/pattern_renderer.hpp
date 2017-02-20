#pragma once
#include <vector>

#include <opengl.h>
#include <glm/glm.hpp>

#include <shader.hpp>
#include <texture.hpp>
#include <chrono>
#include <string>


class PatternRender {
public:
  PatternRender(glm::vec2 canvasSize, const Shader& pattern);

  void render() const;

  const Texture& getTexture() const;
  std::string getName() const;

private:
  GLuint VertexArrayID;
  GLuint vertexbuffer;

  GLuint FramebufferName;
  Texture renderedTexture;
  int width, height;
  const Shader pattern;
  std::string name;

  std::chrono::time_point<std::chrono::high_resolution_clock> start;

  PatternRender(const PatternRender& copy);
};
