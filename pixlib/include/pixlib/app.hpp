#pragma once
#include <vector>
#include <map>
#include <memory>

#include <pixlib/data_model.hpp>
#include <pixlib/pattern.hpp>
#include <pixlib/camera.hpp>
#include <pixlib/led_cluster.hpp>
#include <pixlib/scene.hpp>
#include <pixlib/timer.hpp>
#include <pixlib/fade_candy.hpp>

#include <sstream>

namespace Pixlib {
  class App {
  public:

    App(const Storage& storage);

    float scene_fps();
    float scene_render_time();
    float led_render_time();

    const Texture& get_pattern_texture();

    void process_mouse_movement(int xoffset, int yoffset);
    void process_mouse_scroll(float x);

    void move_perspective_to_camera();
    void set_screen_size(int width, int height);

    void register_pattern(std::string name, std::shared_ptr<Pattern> pattern);

    void set_random_pattern();

    const std::string& get_pattern();
    float pattern_get_time_elapsed();
    void tick();

    float brightness;

    Scene scene;
    IsoCamera camera;
  private:
    std::map<std::string, std::shared_ptr<Pattern> > patterns;
    std::string pattern_name;

    PatternRender pattern_render;

    LedClusterCluster led_clusters;

    IsoCamera viewed_from;

    Storage storage;

  };
}
