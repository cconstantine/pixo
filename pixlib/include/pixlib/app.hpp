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

    App(const Sculpture& sculpture, const std::vector<PatternCode> pattern_codes);
    ~App();

    float scene_fps();
    float scene_render_time();
    float led_render_time();

    void process_mouse_movement(int xoffset, int yoffset);
    void process_mouse_scroll(float x);

    void move_perspective_to_camera();
    void set_screen_size(int width, int height);

    void set_random_pattern();
    void next_pattern();
    void prev_pattern();

    void register_pattern(std::shared_ptr<Pattern> pattern);
    const Pattern& get_pattern();
    const Texture& get_pattern_texture();

    void render_leds();
    void render_scene();

    float brightness;
    float rotation;

    IsoCamera viewed_from;
    IsoCamera camera;
  private:

    std::map<std::string, std::shared_ptr<Pattern> > patterns;
    std::shared_ptr<Pattern> pattern;

    Scene scene;

    LedClusterCluster led_clusters;

  };
}
