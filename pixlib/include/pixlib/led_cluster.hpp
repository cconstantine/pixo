#pragma once

#include <opengl.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <pixlib/cube.hpp>
#include <pixlib/fade_candy.hpp>
#include <pixlib/mesh.hpp>
#include <pixlib/led_mesh.hpp>
#include <pixlib/led_renderer.hpp>
#include <pixlib/pattern_renderer.hpp>
#include <pixlib/pattern.hpp>
#include <pixlib/renderer.hpp>
#include <pixlib/timer.hpp>



namespace Pixlib {
  class LedCluster: public Drawable {
  public:
    LedCluster(FadeCandy *fadecandy, const Texture& pattern_texture);

    // Render the mesh
    virtual void Draw(const Shader& shader);

    void render(const IsoCamera& viewed_from, const Pattern& pattern);

    GLuint numLeds();

    float render_time();

  private:

    Texture led_texture;
    LedRender led_renderer;
    FadeCandy *fadecandy;

    /*  Model Data  */
    LedMesh leds_for_calc;

    Cube leds_for_display;
    Timer render_timer;

    static size_t led_canvas_size(size_t leds);

  };
}
