#include <pixlib/app.hpp>

namespace Pixlib {
  App::App() :
   scene(),
   pattern_render(glm::vec2(1000, 1000))

  {  }

  App::~App() {
    for (LedCluster* i : led_clusters) {
      delete i;
    }
    led_clusters.clear();
  }

  void App::addFadeCandy(FadeCandy* fc) {
    LedCluster *lc = new LedCluster(fc, pattern_render.getTexture());

    scene.addCluster(lc);
    led_clusters.push_back(lc);
  }

  float App::scene_fps() {
    return scene.getFps();
  }

  float App::scene_render_time() {
    return scene.get_render_time();
  }

  float App::led_render_time() {
    float total = 0;
    for (LedCluster* i : led_clusters) {
      total += i->render_time();
    }
    return total;
  }

  void App::ProcessMouseMovement(int xoffset, int yoffset) {
    camera.ProcessMouseMovement(xoffset, yoffset);
  }


  void App::ProcessMouseScroll(float x) {
    camera.ProcessMouseScroll(x);
  }

  void App::move_perspective_to_camera() {
    viewed_from.moveTowards(camera, scene.getTimeDelta()*0.8);
  }

  const Texture& App::getPatternTexture() {
    return pattern_render.getTexture();
  }

  void App::tick(Pattern* pattern, int width, int height) {
    pattern_render.render(*pattern);

    for (LedCluster* led_cluster : led_clusters) {
      led_cluster->render(viewed_from, *pattern);
    }
    scene.render(camera, width, height);
  }
}
