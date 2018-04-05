#pragma once
#include <vector>
#include <map>
#include <memory>

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

    void BuildPixo(const std::vector<FadeCandy*>& fadecandies, unsigned int per_size);

    
    float scene_fps();
    float scene_render_time();
    float led_render_time();

    const Texture& getPatternTexture();

    void ProcessMouseMovement(int xoffset, int yoffset);
    void ProcessMouseScroll(float x);

    void move_perspective_to_camera();
    void setScreenSize(int width, int height);

    std::map<std::string, std::shared_ptr<Pattern> > patterns;

    void tick(Pattern* pattern, float brightness);
  private:
    void addFadeCandy(FadeCandy* fc);

    Scene scene;
    PatternRender pattern_render;

    std::vector<LedCluster*> led_clusters;

    IsoCamera viewed_from;
    IsoCamera camera;

    FadeCandy *fc;
  };
}
