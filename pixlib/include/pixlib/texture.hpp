#pragma once
#include <opengl.h>

namespace Pixlib {
  class Texture {
  public:
    Texture(int width, int height);
    Texture(const Texture& copy);
    ~Texture();

    GLuint id;
    GLenum target;
    int width;
    int height;

    bool isCopy;
  };

}