#pragma once

#include <opengl.h>
#include <mesh.hpp>
#include <led_mesh.hpp>
#include <shader.hpp>
#include <cube.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <renderer.hpp>

using namespace std;

#include <fade_candy.hpp>

class LedCluster: public Drawable {
public:
  // Draws the model, and thus all its meshes
  LedCluster(FadeCandy *fadecandy, const Texture& pattern_texture);


  // Render the mesh
  virtual void Draw(Shader shader);
  
  void render(const IsoCamera& viewed_from, const Shader& pattern);

  const Texture& getPatternTexture();

  GLuint numLeds();



private:
  void addStrip(glm::vec3 start, glm::vec3 end, int divisions);

  Texture led_texture;
  FrameBufferRender fb_render;
  PatternRender pattern_render;
  FadeCandy *fadecandy;

  /*  Model Data  */
  LedMesh leds_for_calc;;

  Cube leds_for_display;

  static size_t led_canvas_size(size_t leds);

};