#pragma once

#include <opengl.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <memory>


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
  class LedCluster {
  public:
    LedCluster(std::shared_ptr<FadeCandy> fadecandy, const Texture& pattern_texture);

    void render(const IsoCamera& viewed_from, float brightness);

    GLuint num_leds();

    float render_time();

    Drawable* get_drawable();

  private:

    Texture led_texture;
    LedRender led_renderer;
    std::shared_ptr<FadeCandy> fadecandy;

    /*  Model Data  */
    LedMesh leds_for_calc;

    Cube leds_for_display;
    Timer render_timer;

    static size_t led_canvas_size(size_t leds);

  };
}
