#pragma once
#include <vector>
#include <chrono>

#include <opengl.h>

#include <pixlib/shader.hpp>
#include <pixlib/cube.hpp>
#include <pixlib/camera.hpp>

namespace Pixlib {
  class ScreenRender {
  public:
    void render(const IsoCamera& perspective);
    std::vector<Drawable*> models;

  private:
    std::chrono::time_point<std::chrono::steady_clock> lastRender;
  };
}
