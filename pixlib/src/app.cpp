#include <pixlib/app.hpp>
#include <glm/ext.hpp>
namespace Pixlib {

  App::App() :
   scene(),
   focal_point(std::make_shared<Cube>()),
   target(glm::vec3(0.0f)), paused(false),
   tracking_camera(false)
  {

    // viewed_from.Yaw = 108.0f;
    // viewed_from.Pitch = 12.0f;
    // viewed_from.Zoom = 2.4f;
    // viewed_from.update_camera_vectors();

    // camera = viewed_from
    // camera.update_camera_vectors();
    // for(const LedGeometry& geom : sculpture.leds) {
    //   std::shared_ptr<LedCluster> lc = std::make_shared<LedCluster>(geom);

    //   scene.add_cluster(lc->get_drawable());
    //   led_clusters.push_back(lc);

    //   for(Point point : *geom.locations.get()) {
    //     if (glm::length(viewed_from.scope) < glm::length(point)) {
    //       viewed_from.scope = glm::abs(point);
    //     }
    //   }
    // }

    focal_point->add_instance(camera.Position, glm::vec2(0.0f), glm::vec3(0.0f));
    scene.add_cluster(focal_point);
  }

  void App::add_fadecandy(const std::string& hostname, const std::list<glm::vec3> leds) {
    std::shared_ptr<LedCluster> lc = std::make_shared<LedCluster>(hostname, leds);
    
    scene.add_cluster(lc->get_drawable());
    led_clusters.push_back(lc);

    for(glm::vec3 point : leds) {
      if (glm::length(viewed_from.scope) < glm::length(point)) {
        viewed_from.scope = glm::abs(point);
      }
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

  std::shared_ptr<Pattern> App::get_pattern() {
    if (patterns.size() == 0) {
    }

    auto pattern = patterns.find(active_pattern);
    if (pattern == patterns.end()) {
      fprintf(stderr, "Warning: active pattern (%s) not found, randomizing\n", active_pattern.c_str());
      active_pattern = random_pattern();
      return get_pattern();
    }
    return pattern->second;
  }

  void App::set_pattern(const std::string& name) {
    if (name != active_pattern) {
      ALOGV("Switching to pattern: %s\n", name.c_str());
      active_pattern = name;
      get_pattern()->reset_start();
    }
  }

  const Texture& App::get_pattern_texture() {
    return get_pattern()->get_texture();
  }

  void App::register_pattern(std::shared_ptr<Pattern> pattern) {
    patterns[pattern->name] = pattern;
  }

  void App::disable_pattern(const std::string& name) {
    auto pattern = patterns.find(name);
    if (pattern != patterns.end()) {
      patterns.erase(pattern);
    }
  }

  std::string App::random_pattern()
  {
    auto it = patterns.begin();
    std::advance(it, rand() % patterns.size());
    return it->first;
  }

  std::string App::next_pattern() {
    auto it = patterns.find(active_pattern);
    it++;

    if (it == patterns.end()) {
      it = patterns.begin();
    }

    return it->first;
  }

  std::string App::prev_pattern() {
    auto it = patterns.find(get_pattern()->name);
    if (it == patterns.begin()) {
      it = patterns.end();
    }
    it--;

    return it->first;
  }

  void App::set_target_location(glm::vec3 target) {
    focal_point->move_instance(0, target);
    this->target = target;
  }

  void App::set_screen_size(int width, int height) {
    camera.width = width;
    camera.height = height;
  }

  void App::render_leds() {
    std::shared_ptr<Pattern> pattern = get_pattern();
    if (target != glm::vec3(0.0f)) {
      viewed_from.move_towards(target, scene.get_time_delta()*20.0);
    }

    if (!paused) {
      pattern->render();
    }

    for (std::shared_ptr<LedCluster> led_cluster : led_clusters) {
      led_cluster->render(*pattern, viewed_from, brightness, gamma);
    }

    // camera.rotate(scene.get_time_delta()*rotation);
  }

  void App::render_scene() {
    focal_point->hidden = glm::distance(camera.Position, target) < 0.1;

    scene.render(camera, brightness);
  }

}
