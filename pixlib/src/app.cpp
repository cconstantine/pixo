#include <pixlib/app.hpp>

namespace Pixlib {
  App::App() :
   scene()
  {  }

  App::~App() {
    for (LedCluster* i : led_clusters) {
      delete i;
    }
    led_clusters.clear();
  }

  void App::addFadeCandy(FadeCandy* fc) {
    LedCluster *lc = new LedCluster(fc);

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

  void App::move_camera_to_perspective() {
    camera.moveTowards(viewed_from, scene.getTimeDelta()*0.8);
  }

  void App::tick(Pattern* pattern, float brightness, int width, int height) {
    frame_timer.end();
    frame_timer.start();
    
    for (LedCluster* led_cluster : led_clusters) {
      led_cluster->render(viewed_from, *pattern, brightness);
    }
    scene.render(camera, width, height);
  }
}
