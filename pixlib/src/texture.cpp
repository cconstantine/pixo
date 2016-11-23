#include <texture.hpp>

Texture::Texture(const Texture& copy) : width(copy.width), height(copy.height), id(copy.id), isCopy(true)
{
    ALOGV("%d: Texture Copy\n", id);
}

Texture::Texture(int width, int height) : width(width), height(height), isCopy(false)
{
  // The texture we're going to render to
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

  ALOGV("%d: Texture Create\n", id);
}


Texture::~Texture()
{

  ALOGV("%d: Texture delete %d\n", id, isCopy);
	if(isCopy)
		return;
	glDeleteTextures(1, &id);
}