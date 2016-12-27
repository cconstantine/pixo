#pragma once
#include <vector>
#include <librealsense/rs.hpp>

#include <glm/glm.hpp>

#include <texture.hpp>
#include <chrono>


class RealsenseReader {
public:
  RealsenseReader();

  void render();

  const Texture& getTexture();
private:
  Texture renderedTexture;
  Texture depthTexture;

  rs::context ctx;
  rs::device *dev;
};
