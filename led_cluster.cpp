#include <led_cluster.hpp>
#include <math.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

LedCluster::LedCluster(const Texture& texture)
: defaultTexture(texture), model("../models/dome.obj"), leds(texture, GL_POINTS),
  ds(7331), buffer_size(0), gamma(0.5)
{
  setGamma(gamma);
  // Assimp::Importer importer;

  // model = importer.ReadFile("../models/dome.obj", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
  // processNode(model->mRootNode, model);
  // this->loadModel("../models/dome.obj");

  float spacing = .3;
  int per_side = 64;
  for(int i = 0;i < per_side;i++) {
    for(int j = 0;j < per_side;j++) {
      glm::vec3 vertex_start(i*spacing, per_side*spacing, j*spacing);
      glm::vec3 vertex_end(i*spacing,   0*spacing,        j*spacing);


      std::string mac("d8:80:39:66:4b:de");

      addStrip(mac, i*j, vertex_start, vertex_end, per_side);
    }
  }

  fprintf(stderr, "LEDS: %d\n", numLeds());
}



// Draws the model, and thus all its meshes
void LedCluster::Draw(Shader shader)
{
  this->leds.Draw(shader);
}



void LedCluster::setGamma(float g) {
  gamma = g;

  for(int i = 0;i < 256;i++) {
    lut[i] = pow(i, gamma);
  }
}

GLuint LedCluster::numLeds() {
  return leds.numVertices();
}

void LedCluster::update(std::vector<uint8_t> &frameBuffer) {

  for(auto i: strip_mappings) {
    std::string mac_address = i.first;
    std::vector<Strip> strips = i.second;

    if(ds.pushers.find(mac_address) != ds.pushers.end()) {
      std::shared_ptr<PixelPusher> pusher = ds.pushers[mac_address];

      for(auto strip: strips) {
        pusher->update(strip.strip, &frameBuffer[strip.offset], strip.size, strip.strip_offset);
      }
      pusher->send();
    }

  }
}



void LedCluster::addStrip(std::string &mac, int strip, glm::vec3 vertex_start, glm::vec3 vertex_end, int divisions) {
  int byte_offset = buffer_size;
  strip_mappings[mac].push_back(Strip(strip, 0, byte_offset, divisions*3));
  buffer_size += divisions*3;

  addStrip(vertex_start, vertex_end, divisions);
}

void LedCluster::addStrip(glm::vec3 vertex_start, glm::vec3 vertex_end, int divisions) {

  glm::vec3 vertex_delta = vertex_end - vertex_start ;


  glm::vec2 texture_start;// = model.meshes[0].getVertex(start).TexCoords;
  glm::vec2 texture_end;//   = model.meshes[0].getVertex(end).TexCoords;

  glm::vec2 texture_delta = texture_end - texture_start;


  for(int i = 0;i < divisions;i++) {
    glm::vec3 ballPosDelta = vertex_start  + vertex_delta  * (1.0f/divisions)*float(i);
    glm::vec2 texDelta     = texture_start + texture_delta * (1.0f/divisions)*float(i);
    
    int count = numLeds();
    int x = count % 1000;
    int y = count / 1000;
    // fprintf(stderr, "x: %3d, y: %3d\n", x, y);
    glm::vec3 planePosDelta((float)x, (float)y, 0.0f);

    Vertex vertex;
    vertex.Position = ballPosDelta; 
    vertex.TexCoords = texDelta;
    vertex.framebuffer_proj = planePosDelta;

    leds.addVertex(vertex);
  }
}


LedCluster::Strip::Strip() :strip(0), strip_offset(0), offset(0), size(0) { }

LedCluster::Strip::Strip(int strip, int strip_offset, int offset, int size) :
  strip(strip), strip_offset(strip_offset), offset(offset), size(size) { }

LedCluster::Strip::Strip(const Strip& copy) :
  strip(copy.strip), strip_offset(copy.strip_offset), offset(copy.offset), size(copy.size) { }