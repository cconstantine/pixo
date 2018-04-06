#include <pixlib/app.hpp>

namespace Pixlib {
  App::App(glm::vec2 canvas_size) :
   scene(),
   pattern_render(canvas_size),
   brightness(1.0f)
  {  }

  void App::BuildPixo(const FadeCandyCluster& fadecandies, unsigned int per_size) {
    float spacing = 0.04318;

    int width = per_size;
    int height = per_size;
    int depth = per_size;

    float x_offset = -(float)width  / 2.0f  + 0.5f;
    float y_offset = -(float)height / 2.0f - 0.5f;
    float z_offset = -(float)depth  / 2.0f  + 0.5f;

    viewed_from.scope = glm::vec3(x_offset, y_offset, z_offset)*-spacing;
    camera.scope      = glm::vec3(x_offset, y_offset, z_offset)*-spacing;

    int num_fadecandies = fadecandies.size();
    int per_fc = height / num_fadecandies;

    for(int y = height;y > 0;y--) {
      int direction = 1;
      int selection = (y-1) / per_fc;

      std::shared_ptr<FadeCandy> fc = fadecandies[selection];

      for(int z = 0;z < height;z++) {
        for(int x = std::max(-direction * (width - 1), 0); x >= 0 && x < width;x+=direction) {
          fc->add_led(glm::vec3( ((float)x + x_offset)*spacing,
                                ((float)z + z_offset)*spacing,
                                ((float)y + y_offset)*spacing));
        }
        direction *= -1;
      }
    }

  }

  void App::add_fadecandy(std::shared_ptr<FadeCandy> fc) {
    std::shared_ptr<LedCluster> lc = std::make_shared<LedCluster>(fc, pattern_render.get_texture());

    scene.add_cluster(lc.get());
    led_clusters.push_back(lc);
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
