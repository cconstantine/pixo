#pragma once
#include <vector>

#include <pixlib/pattern.hpp>
#include <pixlib/camera.hpp>
#include <pixlib/led_cluster.hpp>
#include <pixlib/scene.hpp>
#include <pixlib/timer.hpp>
#include <pixlib/fade_candy.hpp>

namespace Pixlib {
  class App {
  public:

    App(glm::vec2 canvas_size);
    ~App();

    void addFadeCandy(FadeCandy* fc);
    
    float scene_fps();
    float scene_render_time();
    float led_render_time();

    const Texture& getPatternTexture();

    void ProcessMouseMovement(int xoffset, int yoffset);
    void ProcessMouseScroll(float x);

    void move_perspective_to_camera();
    void tick(Pattern* pattern, int width, int height);
  private:
    Scene scene;
    PatternRender pattern_render;

    std::vector<LedCluster*> led_clusters;

    IsoCamera viewed_from;
    IsoCamera camera;

    FadeCandy *fc;
  };
}
