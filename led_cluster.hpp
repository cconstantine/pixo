#pragma once

#include <mesh.hpp>
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


class LedCluster : public Drawable {
public:
  // Draws the model, and thus all its meshes
  LedCluster(const Texture& texture);

  void update(std::vector<uint8_t> &frameBuffer);
  virtual void Draw(Shader shader);


  void setGamma(float g);


  GLuint numLeds();

private:
  /*  Model Data  */
  Model model;
  Mesh leds;
  string directory;
  vector<Texture> textures_loaded;  // Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
  Texture defaultTexture;
  glm::vec2 defaultTexCoords;


  void addStrip(std::string &mac, int strip, int strip_offset, int start, int end, int divisions, glm::vec3 offset);
  void addStrip(int start, int end, int divisions, glm::vec3 offset);

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