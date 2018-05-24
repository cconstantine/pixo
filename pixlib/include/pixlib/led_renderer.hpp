#pragma once
#include <vector>

#include <opengl.h>
#include <glm/glm.hpp>

#include <pixlib/led_mesh.hpp>
#include <pixlib/shader.hpp>
#include <pixlib/camera.hpp>
#include <pixlib/texture.hpp>

namespace Pixlib {
  class LedRender {
  public:
    LedRender(const Texture& renderTo);

    void render(const Texture& texture,  const IsoCamera& perspective, float brightness, uint8_t* data, size_t size);

    LedMesh* leds;

  private:
    GLuint FramebufferName;
    Texture renderedTexture;
    int width, height;

    GLuint pbos[2];
    GLuint active_pbo;

  };
}
