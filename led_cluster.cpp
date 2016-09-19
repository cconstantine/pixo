#include <led_cluster.hpp>
#include <math.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

LedCluster::LedCluster(const Texture& texture)
: balls("../models/ball.obj",  texture),
  plane("../models/plane.obj", texture),
  ds(7331), buffer_size(0), gamma(0.5)
{
  setGamma(gamma);
  Assimp::Importer importer;

  model = importer.ReadFile("../models/dome.obj", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);


  std::string mac3("d8:80:39:66:4b:de");
  addStrip(mac3, 0,  0,  28,  2, 84);
  addStrip(mac3, 0, 84,   2, 22, 82);
  addStrip(mac3, 1,  0,  28, 22, 84);
  addStrip(mac3, 1, 84,  22,  0, 72);
  addStrip(mac3, 2,  0,  28, 24, 84);
  addStrip(mac3, 2, 84,  24, 22, 82);
  addStrip(mac3, 3,  0,  28, 29, 84);
  addStrip(mac3, 3, 84,  29, 24, 82);
  addStrip(mac3, 4,  0,  28, 30, 84);
  addStrip(mac3, 4, 84,  30, 29, 82);
  addStrip(mac3, 5,  0,  28,  4, 84);
  addStrip(mac3, 5, 84,   4, 30, 82);
  addStrip(mac3, 6,  0,  30, 44, 84);
  addStrip(mac3, 6, 84,  44, 29, 84);

  std::string mac2("d8:80:39:66:48:44");
  addStrip(mac2, 0,  0,  23, 22, 84);
  addStrip(mac2, 0, 84,  22, 16, 82);
  addStrip(mac2, 1,  0,  23, 16, 84);
  addStrip(mac2, 1, 84,  16,  0, 72);
  addStrip(mac2, 2,  0,  23, 18, 84);
  addStrip(mac2, 2, 84,  18, 16, 82);
  addStrip(mac2, 3,  0,  23, 25, 84);
  addStrip(mac2, 3, 84,  25, 18, 82);
  addStrip(mac2, 4,  0,  23, 26, 84);
  addStrip(mac2, 4, 84,  26, 25, 82);
  addStrip(mac2, 5,  0,  23, 24, 84);
  addStrip(mac2, 5, 84,  24, 26, 82);
  addStrip(mac2, 6,  0,  26, 41, 84);
  addStrip(mac2, 6, 84,  41, 25, 84);

  std::string mac1("d8:80:39:65:f1:91");
  addStrip(mac1, 0,  0,  17, 16, 84);
  addStrip(mac1, 0, 84,  16, 10, 82);
  addStrip(mac1, 1,  0,  17, 10, 84);
  addStrip(mac1, 1, 84,  10,  0, 72);
  addStrip(mac1, 2,  0,  17, 12, 84);
  addStrip(mac1, 2, 84,  12, 10, 82);
  addStrip(mac1, 3,  0,  17, 19, 84);
  addStrip(mac1, 3, 84,  19, 12, 82);
  addStrip(mac1, 4,  0,  17, 20, 84);
  addStrip(mac1, 4, 84,  20, 19, 82);
  addStrip(mac1, 5,  0,  17, 18, 84);
  addStrip(mac1, 5, 84,  18, 20, 82);
  addStrip(mac1, 6,  0,  20, 38, 84);
  addStrip(mac1, 6, 84,  38, 19, 84);
  
  std::string mac5("d8:80:39:66:0d:7e");
  addStrip(mac5, 0,  0,  11, 10, 84);
  addStrip(mac5, 0, 84,  10,  1, 82);
  addStrip(mac5, 1,  0,  11,  1, 84);
  addStrip(mac5, 1, 84,   1,  0, 72);
  addStrip(mac5, 2,  0,  11,  5, 84);
  addStrip(mac5, 2, 84,   5,  1, 82);
  addStrip(mac5, 3,  0,  11, 13, 84);
  addStrip(mac5, 3, 84,  13,  5, 82);
  addStrip(mac5, 4,  0,  11, 14, 84);
  addStrip(mac5, 4, 84,  14, 13, 82);
  addStrip(mac5, 5,  0,  11, 12, 84);
  addStrip(mac5, 5, 84,  12, 14, 82);
  addStrip(mac5, 6,  0,  14, 35, 84);
  addStrip(mac5, 6, 84,  35, 13, 84);

  std::string mac4("d8:80:39:66:29:c6");
  addStrip(mac4, 0,  0,   3,  1, 84);
  addStrip(mac4, 0, 84,   1,  2, 82);
  addStrip(mac4, 1,  0,   3,  2, 84);
  addStrip(mac4, 1, 84,   2,  0, 72);
  addStrip(mac4, 2,  0,   3,  4, 84);
  addStrip(mac4, 2, 84,   4,  2, 82);
  addStrip(mac4, 3,  0,   3,  6, 84);
  addStrip(mac4, 3, 84,   6,  4, 82);
  addStrip(mac4, 4,  0,   3,  8, 84);
  addStrip(mac4, 4, 84,   8,  6, 82);
  addStrip(mac4, 5,  0,   3,  5, 84);
  addStrip(mac4, 5, 84,   5,  8, 82);
  addStrip(mac4, 6,  0,   8, 32, 84);
  addStrip(mac4, 6, 84,  32,  6, 84);
}

void LedCluster::setGamma(float g) {
  gamma = g;

  for(int i = 0;i < 256;i++) {
    lut[i] = pow(i, gamma);
  }
}

void LedCluster::update(std::vector<uint8_t> &frameBuffer) {
  uint8_t buff[frameBuffer.size()];
  for(int i = 0;i < frameBuffer.size();i++) {
    buff[i] = lut[frameBuffer[i]];
  }
  for(auto i: strip_mappings) {
    std::string mac_address = i.first;
    std::vector<Strip> strips = i.second;

    if(ds.pushers.find(mac_address) != ds.pushers.end()) {
      std::shared_ptr<PixelPusher> pusher = ds.pushers[mac_address];

      for(auto strip: strips) {
        pusher->update(strip.strip, &buff[strip.offset], strip.size, strip.strip_offset);
      }
      pusher->send();
    }

  }
}

void LedCluster::addStrip(std::string &mac, int strip, int strip_offset, int start, int end, int divisions) {
  int byte_offset = buffer_size;
  strip_mappings[mac].push_back(Strip(strip, strip_offset*3, byte_offset, divisions*3));
  buffer_size += divisions*3;

  addStrip(start, end, divisions);
}

void LedCluster::addStrip(int start, int end, int divisions) {
  glm::vec3 vertex_start = glm::vec3(
    model->mMeshes[0]->mVertices[start].x,
    model->mMeshes[0]->mVertices[start].y,
    model->mMeshes[0]->mVertices[start].z);
  glm::vec3 vertex_end = glm::vec3(
    model->mMeshes[0]->mVertices[end].x,
    model->mMeshes[0]->mVertices[end].y,
    model->mMeshes[0]->mVertices[end].z);
  glm::vec3 vertex_delta = vertex_end - vertex_start;


  glm::vec2 texture_start = glm::vec2(
    model->mMeshes[0]->mTextureCoords[0][start].x,
    model->mMeshes[0]->mTextureCoords[0][start].y);
  glm::vec2 texture_end   = glm::vec2(
    model->mMeshes[0]->mTextureCoords[0][end].x,
    model->mMeshes[0]->mTextureCoords[0][end].y);

  glm::vec2 texture_delta = texture_end - texture_start;


  for(int i = 0;i < divisions;i++) {
    glm::vec3 ballPosDelta = vertex_start  + vertex_delta  * (1.0f/divisions)*float(i);
    glm::vec2 texDelta     = texture_start + texture_delta * (1.0f/divisions)*float(i);
    
    int count = plane.numInstances();
    int x = count % 1000;
    int y = count / 1000;
    glm::vec3 planePosDelta((float)x, (float)y, 0.0f);

    balls.addInstance(ballPosDelta, texDelta, ballPosDelta);
    plane.addInstance(planePosDelta, texDelta, ballPosDelta);
  }
}


LedCluster::Strip::Strip() :strip(0), strip_offset(0), offset(0), size(0) { }

LedCluster::Strip::Strip(int strip, int strip_offset, int offset, int size) :
  strip(strip), strip_offset(strip_offset), offset(offset), size(size) { }

LedCluster::Strip::Strip(const Strip& copy) :
  strip(copy.strip), strip_offset(copy.strip_offset), offset(copy.offset), size(copy.size) { }