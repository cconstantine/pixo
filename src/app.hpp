#pragma once

#include <pattern.hpp>
#include <camera.hpp>
#include <led_cluster.hpp>
#include <scene.hpp>
#include <timer.hpp>
#include <fade_candy.hpp>

class App {
public:

  App(FadeCandy* fc);

  void add_pattern(Pattern* pattern);

  float scene_fps();
  float scene_render_time();
  float led_render_time();

  const Pattern* current_pattern();
  const Texture& getPatternTexture();

  void ProcessMouseMovement(int xoffset, int yoffset);
  void ProcessMouseScroll(float x);

  void move_perspective_to_camera();
  void change_pattern();

  void tick(int width, int height);
private:
  Scene scene;
  Pattern *pattern;
  LedCluster domeLeds;

  IsoCamera viewed_from;
  IsoCamera camera;

  std::chrono::time_point<std::chrono::high_resolution_clock> last_pattern_change;

  std::vector<Pattern*> patterns;
  FadeCandy *fc;
};