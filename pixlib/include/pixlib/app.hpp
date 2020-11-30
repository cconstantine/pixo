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

    App(const Sculpture& sculpture);
    ~App();

    float scene_fps();
    float scene_render_time();
    float led_render_time();

    void process_mouse_movement(int xoffset, int yoffset);
    void process_mouse_scroll(float x);

    void move_perspective_to_camera();
    void set_screen_size(int width, int height);


    std::shared_ptr<Pattern> get_pattern();
    void set_pattern(const std::string& name);
    const Texture& get_pattern_texture();
    void register_pattern(std::shared_ptr<Pattern> pattern);

    std::string random_pattern();
    std::string next_pattern();
    std::string prev_pattern();

    void set_target_location(glm::vec3 target);


    void render_leds();
    void render_scene();

    float brightness;
    float rotation;
    float gamma;

    IsoCamera viewed_from;
    IsoCamera camera;

    bool paused;
  private:
    std::map<std::string, std::shared_ptr<Pattern> > patterns;
    std::string active_pattern;

    Scene scene;

    LedClusterCluster led_clusters;

    std::shared_ptr<Cube> focal_point;
    glm::vec3 target;
  };
}
