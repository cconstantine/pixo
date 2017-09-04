#include <app.hpp>

App::App(FadeCandy *fc) :
 domeLeds(fc),
 scene(&domeLeds),
 fc(fc),
 pattern(NULL),
 last_pattern_change(std::chrono::high_resolution_clock::now())
{ }

void App::add_pattern(Pattern* p) {
  patterns.push_back(p);
  if(pattern == NULL) {
    pattern = p;
  }
}

float App::scene_fps() {
  return scene.getFps();
}

float App::scene_render_time() {
  return scene.get_render_time();
}

float App::led_render_time() {
  return domeLeds.render_time();
}

const Pattern* App::current_pattern() {
  return pattern;
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

void App::change_pattern() {
  pattern = patterns[rand() % patterns.size()];
  pattern->resetStart();
  last_pattern_change = std::chrono::high_resolution_clock::now();
}

void App::tick(int width, int height) {
  camera.rotate(scene_render_time() * 5);

  std::chrono::duration<float> diff = std::chrono::high_resolution_clock::now() - last_pattern_change;
  if(diff.count() > 600) {
    change_pattern();
  }


  domeLeds.render(viewed_from, *pattern);
  scene.render(camera, width, height);
}
