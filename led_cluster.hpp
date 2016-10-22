#pragma once

#include <mesh.hpp>
#include <led_mesh.hpp>
#include <shader.hpp>
#include <model.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

#include <pixelpusher/pixel_pusher.hpp>


class LedCluster {
public:
  // Draws the model, and thus all its meshes
  LedCluster(const Texture& texture);

  void update(std::vector<uint8_t> &frameBuffer);


  void setGamma(float g);


  GLuint numLeds();
  /*  Model Data  */
  LedMesh leds_for_calc;;

  Mesh leds_for_display;

private:


  void addStrip(std::string &mac, int strip, glm::vec3 start, glm::vec3 end, int divisions);
  void addStrip(glm::vec3 start, glm::vec3 end, int divisions);

  DiscoveryService ds;

  float gamma;
  unsigned char lut[256];
  class Strip {
  public:
    Strip();
    Strip(int strip, int strip_offset, int offset, int size);
    Strip(const Strip& copy);

    int strip, strip_offset, offset;
    size_t size;

  };

  std::map<std::string, std::vector<Strip>> strip_mappings;

  size_t buffer_size;
};