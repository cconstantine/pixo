#pragma once
#include <chrono>

#include <opengl.h>

#include <pixlib/cube.hpp>
#include <pixlib/camera.hpp>
#include <pixlib/renderer.hpp>
#include <pixlib/led_cluster.hpp>
#include <pixlib/timer.hpp>

namespace Pixlib {
  class Scene {
  public:
    Scene();

    void add_cluster(std::shared_ptr<Drawable> leds);

    void render(const IsoCamera& perspective, float brightness);

    float get_fps();

    float get_time_delta();

    float get_render_time();
  private:
    ScreenRender screen;
    
    std::chrono::duration<float> deltaTime;

    std::chrono::time_point<std::chrono::steady_clock> lastTime;
    std::chrono::time_point<std::chrono::steady_clock> lastUpdate;
    float fps;
    unsigned int frames;
    bool next;

    Timer render_timer;
  };
}
