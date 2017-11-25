#pragma once

#include <opengl.h>
#include <pixlib/shader.hpp>

namespace Pixlib {
  class Drawable {
  public:
    virtual void Draw(const Shader& shader) = 0;
  };
}
