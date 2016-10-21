#include <led_cluster.hpp>
#include <math.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

LedCluster::LedCluster(const Texture& texture)
: leds_for_calc(texture, GL_POINTS), leds_for_display(GL_POINTS),
  ds(7331), buffer_size(0), gamma(0.5)
{
  setGamma(gamma);


  float spacing = .3;

  int width = 32;
  int height = 32;
  int depth = 32;
  for(int i = 0;i < width;i++) {
    for(int j = 0;j < depth;j++) {
      glm::vec3 vertex_start(i*spacing, 0*spacing,      j*spacing);
      glm::vec3 vertex_end(  i*spacing, height*spacing, j*spacing);


      std::string mac("d8:80:39:66:4b:de");

      addStrip(mac, i+(width*j), vertex_start, vertex_end, height);
    }
  }

  fprintf(stderr, "LEDS: %d\n", numLeds());
}

void LedCluster::setGamma(float g) {
  gamma = g;

  for(int i = 0;i < 256;i++) {
    lut[i] = pow(i, gamma);
  }
}

GLuint LedCluster::numLeds() {
  return leds_for_calc.numVertices();
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
    int x = count % 256;
    int y = count / 256;
    glm::vec3 planePosDelta((float)x + 0.5f, (float)y + 0.5f, 0.0f);

    Vertex vertex_calc;
    vertex_calc.Position = ballPosDelta; 
    vertex_calc.TexCoords = texDelta;
    vertex_calc.framebuffer_proj = planePosDelta;

    leds_for_calc.addVertex(vertex_calc);

    // fprintf(stderr, "x: %3d, y: %3d\n", x, y);
    // fprintf(stderr, "x: %4.1f, y: %4.1f, z: %4.1f\n", ballPosDelta.x, ballPosDelta.y, ballPosDelta.z);


    Vertex vertex_display;
    vertex_display.Position = ballPosDelta;
    vertex_display.TexCoords = glm::vec2(((float)x + 0.5) / 256, ((float)y + 0.5) / 256);
    vertex_display.framebuffer_proj = planePosDelta;

    leds_for_display.addVertex(vertex_display);
  }
}


LedCluster::Strip::Strip() :strip(0), strip_offset(0), offset(0), size(0) { }

LedCluster::Strip::Strip(int strip, int strip_offset, int offset, int size) :
  strip(strip), strip_offset(strip_offset), offset(offset), size(size) { }

LedCluster::Strip::Strip(const Strip& copy) :
  strip(copy.strip), strip_offset(copy.strip_offset), offset(copy.offset), size(copy.size) { }