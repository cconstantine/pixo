#pragma once

#include <opengl.h>
#include <pixlib/shader.hpp>
#include <pixlib/camera.hpp>

namespace Pixlib {
  class Drawable {
  public:
    virtual void draw(const IsoCamera& perspective) = 0;
  };
}
