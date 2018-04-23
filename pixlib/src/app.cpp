#include <pixlib/app.hpp>

namespace Pixlib {

  App::App(const Sculpture& sculpture) :
   scene(),
   pattern_render(glm::vec2(sculpture.canvas_width, sculpture.canvas_height)),
   brightness(1.0f)
  {
    viewed_from.scope = sculpture.scope;
    camera.scope      = sculpture.scope;

    for(const LedGeometry& geom : sculpture.leds) {
      std::shared_ptr<LedCluster> lc = std::make_shared<LedCluster>(geom, pattern_render.get_texture());

      scene.add_cluster(lc->get_drawable());
      led_clusters.push_back(lc);
    }
  }

  float App::scene_fps() {
    return scene.get_fps();
  }

  float App::scene_render_time() {
    return scene.get_render_time();
  }

  float App::led_render_time() {
    float total = 0;
    for (std::shared_ptr<LedCluster> i : led_clusters) {
      total += i->render_time();
    }
    return total;
  }

  void App::process_mouse_movement(int xoffset, int yoffset) {
    camera.process_mouse_movement(xoffset, yoffset);
  }

  void App::process_mouse_scroll(float x) {
    camera.process_mouse_scroll(x);
  }

  void App::move_perspective_to_camera() {
    viewed_from.move_towards(camera, scene.get_time_delta()*0.8);
  }

  const Texture& App::get_pattern_texture() {
    return pattern_render.get_texture();
  }

  void App::register_pattern(std::string name, std::shared_ptr<Pattern> pattern)
  {
    if(patterns.size() == 0) {
      pattern_name = name;
    }
    patterns[name] = pattern;
  }

  void App::set_random_pattern()
  {
    auto it = patterns.begin();
    std::advance(it, rand() % patterns.size());
    pattern_name = it->first;
  }

  const std::string& App::get_pattern()
  {
    return pattern_name;
  }

  void App::set_screen_size(int width, int height) {
    camera.width = width;
    camera.height = height;
  }

  void App::tick() {
    pattern_render.render(*patterns[pattern_name].get());

    for (std::shared_ptr<LedCluster> led_cluster : led_clusters) {
      led_cluster->render(viewed_from, brightness);
    }
    scene.render(camera);
  }
}
