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
  LedCluster domeLeds;

  IsoCamera viewed_from;
  IsoCamera camera;

  FadeCandy *fc;
};