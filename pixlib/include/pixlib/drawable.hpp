#pragma once

#include <opengl.h>
#include <pixlib/shader.hpp>
#include <pixlib/camera.hpp>

namespace Pixlib {
  class Drawable {
  public:
    virtual void draw(const IsoCamera& perspective) = 0;
    virtual int  add_instance(glm::vec3 posDelta, glm::vec2 texDelta, glm::vec3 projDelta) = 0;
  };
}
