#include <texture.hpp>

Texture::Texture(const Texture& copy) : width(copy.width), height(copy.height), id(copy.id), isCopy(true)
{ }

Texture::Texture(int width, int height) : width(width), height(height), isCopy(false)
{
  // The texture we're going to render to
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
}


Texture::~Texture()
{
	if(isCopy)
		return;
	glDeleteTextures(1, &id);
}



Texture3d::Texture3d(const Texture3d& copy) :
 width(copy.width),
 height(copy.height),
 depth(copy.depth),
 id(copy.id),
 isCopy(true)
{ }

Texture3d::Texture3d(int width, int height, int depth) :
 width(width),
 height(height),
 depth(depth),
 isCopy(false)
{
  // The texture we're going to render to
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_3D, id);
  glTexImage3D(GL_TEXTURE_3D,
    0,
    GL_DEPTH_COMPONENT,

   width,
   height,
   depth,
  0,
  GL_DEPTH_COMPONENT,
  GL_UNSIGNED_SHORT,
  nullptr);
}


Texture3d::~Texture3d()
{
	if(isCopy)
		return;
	glDeleteTextures(1, &id);
}