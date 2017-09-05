#pragma once
#include <vector>
#include <chrono>

#include <opengl.h>

#include <pixlib/shader.hpp>
#include <pixlib/cube.hpp>
#include <pixlib/camera.hpp>

namespace Pixlib {
  class SceneRender {
  protected:
    SceneRender();
  };

  class ScreenRender : public SceneRender {

  public:
    ScreenRender();

    void render(const IsoCamera& perspective, int width, int height);
    std::vector<Drawable*> models;

  private:

    Shader shader;
    void setupLights(const IsoCamera& perspective);
    std::chrono::time_point<std::chrono::steady_clock> lastRender;

  };
}
