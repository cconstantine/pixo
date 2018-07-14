#include <pixlib/app.hpp>
#include <glm/ext.hpp>

namespace Pixlib {

  App::App(const Storage& storage) :
   storage(storage),
   scene(),
   brightness(storage.sculpture.brightness),
   rotation(storage.sculpture.rotation)
   focal_point(std::make_shared<Cube>(Texture(1,1)))
  {
    viewed_from = storage.sculpture.projection_perspective;
    camera      = storage.sculpture.camera_perspective;

    for(const LedGeometry& geom : storage.sculpture.leds) {
      std::shared_ptr<LedCluster> lc = std::make_shared<LedCluster>(geom);

      scene.add_cluster(lc->get_drawable());
      led_clusters.push_back(lc);
    }

    for(const PatternCode& pattern : this->storage.patterns()) {
      register_pattern(std::make_shared<Pattern>(pattern.name, pattern.shader_code.c_str()));
    }

    if (patterns.find(storage.sculpture.active_pattern_name) != patterns.end()) {
      this->pattern = this->patterns[storage.sculpture.active_pattern_name];
    }

    focal_point->add_instance(glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f));
    scene.add_cluster(focal_point);
  }

  App::~App() {
    storage.sculpture.camera_perspective.yaw = camera.Yaw;
    storage.sculpture.camera_perspective.pitch = camera.Pitch;
    storage.sculpture.camera_perspective.zoom = camera.Zoom;
    storage.sculpture.camera_perspective.scope = camera.scope;

    storage.sculpture.projection_perspective.yaw = viewed_from.Yaw;
    storage.sculpture.projection_perspective.pitch = viewed_from.Pitch;
    storage.sculpture.projection_perspective.zoom = viewed_from.Zoom;
    storage.sculpture.projection_perspective.scope = viewed_from.scope;

    storage.sculpture.active_pattern_name = get_pattern().name;
    storage.sculpture.brightness = brightness;
    storage.sculpture.rotation = rotation;
    storage.save_app_state();
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
    //viewed_from.move_towards(camera, scene.get_time_delta()*0.8);
  }

  void App::register_pattern(std::shared_ptr<Pattern> pattern)
  {
    this->pattern = pattern;
    patterns[pattern->name] = pattern;
  }

  void App::set_random_pattern()
  {
    auto it = patterns.begin();
    std::advance(it, rand() % patterns.size());
    pattern = it->second;
    pattern->reset_start();
  }

  void App::next_pattern() {
    auto it = patterns.find(get_pattern().name);
    it++;

    if (it == patterns.end()) {
      it = patterns.begin();
    }

    pattern = it->second;
    pattern->reset_start();
  }

  void App::prev_pattern() {
    auto it = patterns.find(get_pattern().name);
    if (it == patterns.begin()) {
      it = patterns.end();
    }
    it--;

    pattern = it->second;
    pattern->reset_start();

  }

  const Pattern& App::get_pattern()
  {
    return *pattern.get();
  }
 
  const Texture& App::get_pattern_texture() {
    return pattern->get_texture();
  }

  void App::set_screen_size(int width, int height) {
    camera.width = width;
    camera.height = height;
  }

  void App::render_leds() {
    glm::vec3 face;
    
    if (face_finder.tick(face)) {
      face += glm::vec3(0.0f, viewed_from.scope.y, viewed_from.scope.z);
      ALOGV("face: %s\n", glm::to_string(face).c_str());

      focal_point->move_instance(0, face);
      viewed_from.Position = face;
    } 
    pattern->render();

    for (std::shared_ptr<LedCluster> led_cluster : led_clusters) {
      led_cluster->render(get_pattern(), viewed_from, brightness);
    }

    camera.rotate(scene.get_time_delta()*rotation);
  }

  void App::render_scene() {
    scene.render(camera);
  }
}
