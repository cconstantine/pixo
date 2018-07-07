#pragma once
#include <vector>
#include <chrono>
#include <memory>

#include <opengl.h>

#include <pixlib/shader.hpp>
#include <pixlib/cube.hpp>
#include <pixlib/camera.hpp>

namespace Pixlib {
  class ScreenRender {
  public:
    ScreenRender();

    void render(const IsoCamera& perspective);
    std::vector<std::shared_ptr<Drawable>> models;

  private:
    std::chrono::time_point<std::chrono::high_resolution_clock> lastRender;
  };
}
