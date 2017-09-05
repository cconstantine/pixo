#include <pixlib/app.hpp>

namespace Pixlib {
  App::App(FadeCandy *fc) :
   domeLeds(fc),
   scene(&domeLeds),
   fc(fc)
  { }


  float App::scene_fps() {
    return scene.getFps();
  }

  float App::scene_render_time() {
    return scene.get_render_time();
  }

  float App::led_render_time() {
    return domeLeds.render_time();
  }

  const Texture& App::getPatternTexture() {
    return domeLeds.getPatternTexture();
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

    domeLeds.render(viewed_from, *pattern);
    scene.render(camera, width, height);
  }
}
