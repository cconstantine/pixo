#include <pixlib/app.hpp>

namespace Pixlib {
  App::App(FadeCandy *fc) :
   led_cluster(fc),
   scene(),
   fc(fc)
  {
    scene.addCluster(&led_cluster);
  }


  float App::scene_fps() {
    return scene.getFps();
  }

  float App::scene_render_time() {
    return scene.get_render_time();
  }

  float App::led_render_time() {
    return led_cluster.render_time();
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

  void App::tick(Pattern* pattern, int width, int height) {
    camera.rotate(scene_render_time() * 5);

    led_cluster.render(viewed_from, *pattern);
    scene.render(camera, width, height);
  }
}
