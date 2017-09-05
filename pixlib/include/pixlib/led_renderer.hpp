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

    void render(const IsoCamera& perspective, uint8_t* data, size_t size);

    Texture getTexture();

    LedMesh* leds;

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
}
