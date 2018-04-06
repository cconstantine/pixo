#pragma once
#include <vector>

#include <opengl.h>
#include <glm/glm.hpp>

#include <pixlib/pattern.hpp>
#include <pixlib/texture.hpp>

namespace Pixlib {
  class PatternRender {
  public:
    PatternRender(glm::vec2 canvasSize);

    void render(const Pattern& pattern);

    const Texture& get_texture();
  private:
    GLuint VertexArrayID;
    GLuint vertexbuffer;

    GLuint FramebufferName;
    Texture renderedTexture;
    int width, height;


  };
}
