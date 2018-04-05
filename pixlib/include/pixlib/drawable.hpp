#pragma once

#include <opengl.h>
#include <pixlib/shader.hpp>
#include <pixlib/camera.hpp>

namespace Pixlib {
  class Drawable {
  public:
    virtual void Draw(const IsoCamera& perspective) = 0;
  };
}
