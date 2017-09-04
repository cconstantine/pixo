#pragma once

#include <opengl.h>
#include <shader.hpp>

class Drawable {
public:
  virtual void Draw(Shader shader) = 0;
};