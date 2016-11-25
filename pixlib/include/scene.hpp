#pragma once
#include <chrono>

#include <opengl.h>

#include <cube.hpp>
#include <camera.hpp>
#include <renderer.hpp>
#include <led_cluster.hpp>

class Scene {
public:
  Scene(LedCluster* leds);

  void render(const IsoCamera& perspective, int width, int height);

  float getFps();

private:
  ScreenRender screen;
  
  LedCluster* leds;

  std::chrono::duration<float> deltaTime;

  std::chrono::time_point<std::chrono::steady_clock> lastTime;
  std::chrono::time_point<std::chrono::steady_clock> lastUpdate;
  float fps;
  unsigned int frames;
  bool next;

};