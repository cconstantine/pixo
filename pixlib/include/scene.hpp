#pragma once
#include <chrono>

#include <opengl.h>

#include <cube.hpp>
#include <camera.hpp>
#include <renderer.hpp>
#include <led_cluster.hpp>

class Scene {
public:
  Scene(ScreenRender* screen, LedCluster* leds);

  void render(const Shader& pattern, int width, int height);

  void Do_Movement(const bool *keys);
  void orbit_callback(double xoffset, double yoffset);

  void mouse_callback(double xoffset, double yoffset);
  void zoom(double yoffset);

  void matchViewToPerspective();

  float getFps();
  IsoCamera perspective;

private:
  IsoCamera viewed_from;
  ScreenRender* screen;
  
  LedCluster* leds;

  Cube flag;
  
  std::chrono::duration<float> deltaTime;

  std::chrono::time_point<std::chrono::steady_clock> lastTime;
  std::chrono::time_point<std::chrono::steady_clock> lastUpdate;
  float fps;
  unsigned int frames;
  bool next;

  float rot_x, rot_y, rot_z;
};