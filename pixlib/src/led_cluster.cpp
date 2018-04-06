#include <pixlib/led_cluster.hpp>
#include <math.h>

#include <pixlib/dot.hpp>

using namespace std;

namespace Pixlib {
  size_t LedCluster::led_canvas_size(size_t leds)
  {
    return pow(2, ceil(log(sqrt(leds))/log(2)));
  }


  LedCluster::LedCluster(std::shared_ptr<FadeCandy> fadecandy, const Texture& pattern_texture) :
   led_texture(led_canvas_size(fadecandy->get_leds().size()), led_canvas_size(fadecandy->get_leds().size())),
   leds_for_calc(pattern_texture),
   leds_for_display(std::make_shared<Cube>(led_texture)),
   led_renderer(led_texture),
   fadecandy(fadecandy),
   render_timer(120)
  {
    int width = led_texture.width;
    int height = led_texture.height;

    glm::mat4 projection = glm::ortho(0.0f, (float)width, 0.0f, (float)height);

    for(LedInfo led_info : this->fadecandy->get_leds()) {

      glm::vec3 ballPosDelta = led_info.position;
      int count = num_leds();
      int x = count % width;
      int y = count / height;
      glm::vec4 planePosDelta((float)x + 0.5f, (float)y + 0.5f, 0.0f, 1.0f);

      LedVertex vertex_calc;
      vertex_calc.Position = ballPosDelta;
      vertex_calc.framebuffer_proj = projection * planePosDelta;

      leds_for_calc.add_vertex(vertex_calc);

      leds_for_display->add_instance(ballPosDelta, glm::vec2(((float)x + 0.5) / width, ((float)y + 0.5) / height), glm::vec3());
    }


    leds_for_calc.setup_mesh();
    led_renderer.leds = &leds_for_calc;
  }

  GLuint LedCluster::num_leds() {
    return leds_for_calc.num_vertices();
  }

  std::shared_ptr<Drawable> LedCluster::get_drawable() {
    return leds_for_display;
  }

  void LedCluster::render(const IsoCamera& viewed_from, float brightness)
  {
    render_timer.start();
    led_renderer.render(viewed_from, brightness, fadecandy->get_data(), num_leds()*3);
    render_timer.end();

    fadecandy->update();
  }

  float LedCluster::render_time()
  {
    return render_timer.duration();
  }

}
