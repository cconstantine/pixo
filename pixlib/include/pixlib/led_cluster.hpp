#pragma once

#include <opengl.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <memory>


#include <pixlib/data_model.hpp>
#include <pixlib/cube.hpp>
#include <pixlib/fade_candy.hpp>
#include <pixlib/mesh.hpp>
#include <pixlib/led_mesh.hpp>
#include <pixlib/led_renderer.hpp>
#include <pixlib/pattern.hpp>
#include <pixlib/renderer.hpp>
#include <pixlib/timer.hpp>

#include <list>


namespace Pixlib {

  class LedCluster {
  public:
    LedCluster(const std::string& fadecandy_host, const std::list<glm::vec3>& leds);

    std::shared_ptr<Drawable> get_drawable();

    void render(const Pattern& pattern, const IsoCamera& viewed_from, float brightness, float gamma);

    GLuint num_leds();

    float render_time();

  private:

    Texture led_texture;
    LedRender led_renderer;
    std::shared_ptr<FadeCandy> fadecandy;

    /*  Model Data  */
    LedMesh leds_for_calc;

    std::shared_ptr<Drawable> leds_for_display;
    Timer render_timer;

    static size_t led_canvas_size(size_t leds);

  };

  typedef  std::vector<std::shared_ptr<LedCluster>> LedClusterCluster;
}
