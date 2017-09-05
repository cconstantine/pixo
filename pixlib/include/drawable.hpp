#pragma once

#include <opengl.h>
#include <shader.hpp>

class Drawable {
public:
  virtual void Draw(const Shader& shader) = 0;
};