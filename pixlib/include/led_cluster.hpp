#pragma once

#include <opengl.h>
#include <mesh.hpp>
#include <led_mesh.hpp>
#include <pattern.hpp>
#include <cube.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <renderer.hpp>
#include <led_renderer.hpp>
#include <pattern_renderer.hpp>
#include <timer.hpp>


#include <fade_candy.hpp>

class LedCluster: public Drawable {
public:
  // Draws the model, and thus all its meshes
  LedCluster(FadeCandy *fadecandy);


  // Render the mesh
  virtual void Draw(Shader shader);

  void render(const IsoCamera& viewed_from, const Pattern& pattern);

  const Texture& getPatternTexture();

  GLuint numLeds();

  float render_time();

private:

  Texture led_texture;
  LedRender fb_render;
  PatternRender pattern_render;
  FadeCandy *fadecandy;

  /*  Model Data  */
  LedMesh leds_for_calc;;

  Cube leds_for_display;
  Timer render_timer;

  static size_t led_canvas_size(size_t leds);

};