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

  class Texture3d
  {
  public:
    Texture3d(int width, int height, int depth);
    Texture3d(const Texture3d& copy);
    ~Texture3d();

    GLuint id;
    GLenum target;
    int width;
    int height;
    int depth;

    bool isCopy;
  };
}