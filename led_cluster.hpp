#pragma once

#include <mesh.hpp>
#include <led_mesh.hpp>
#include <shader.hpp>
#include <model.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <renderer.hpp>

using namespace std;

#include <pixelpusher/pixel_pusher.hpp>
#include <fade_candy.hpp>

class LedCluster {
public:
  // Draws the model, and thus all its meshes
  LedCluster(FadeCandy *fadecandy, const Texture& pattern_texture, const Texture& led_texture);


  void render(const IsoCamera& viewed_from, const Shader& pattern);

  const Texture& getPatternTexture();

  GLuint numLeds();
  /*  Model Data  */
  LedMesh leds_for_calc;;

  Model leds_for_display;



private:
  void addStrip(glm::vec3 start, glm::vec3 end, int divisions);
  FrameBufferRender fb_render;

  PatternRender pattern_render;

  FadeCandy *fadecandy;
};