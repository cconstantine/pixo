#pragma once
#include <vector>

#include <opengl.h>
#include <glm/glm.hpp>

#include <led_mesh.hpp>
#include <shader.hpp>
#include <camera.hpp>
#include <texture.hpp>
#include <chrono>


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

