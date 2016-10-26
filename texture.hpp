#pragma once
#include <GL/glew.h>

class Texture
{
public:
  Texture(int width, int height);
  Texture(const Texture& copy);

  GLuint id;
  GLenum target;
  int width;
  int height;
};
