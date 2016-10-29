#include <led_cluster.hpp>
#include <math.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

LedCluster::LedCluster(FadeCandy *fadecandy, const Texture& texture, const Texture& led_texture) :
 leds_for_calc(texture),
 leds_for_display("../models/cube.obj", led_texture),
 fb_render(led_texture),
 pattern_render(texture),
 fadecandy(fadecandy)
{

  int width = leds_for_display.getDefaultTexture().width;
  int height = leds_for_display.getDefaultTexture().height;

  for(int i = 0;i < this->fadecandy->getLeds().size();i++) {

    glm::vec3 ballPosDelta = this->fadecandy->getLeds()[i];
    
    int count = numLeds();
    int x = count % width;
    int y = count / height;
    glm::vec3 planePosDelta((float)x + 0.5f, (float)y + 0.5f, 0.0f);

    LedVertex vertex_calc;
    vertex_calc.Position = ballPosDelta; 
    vertex_calc.framebuffer_proj = planePosDelta;

    leds_for_calc.addVertex(vertex_calc);

    // fprintf(stderr, "x: %3d, y: %3d\n", x, y);
    // fprintf(stderr, "x: %4.1f, y: %4.1f, z: %4.1f\n", ballPosDelta.x, ballPosDelta.y, ballPosDelta.z);

    leds_for_display.addInstance(ballPosDelta, glm::vec2(((float)x + 0.5) / width, ((float)y + 0.5) / height), glm::vec3());
  }

  leds_for_calc.setupMesh();
  fb_render.models.push_back(&leds_for_calc);
  fprintf(stderr, "LEDS: %d\n", numLeds());
}

GLuint LedCluster::numLeds() {

  return leds_for_calc.numVertices();
}


void LedCluster::render(const IsoCamera& viewed_from, const Shader& pattern) 
{
  pattern_render.render(pattern);
  fb_render.render(viewed_from, fadecandy->getData(), numLeds()*3);
  fadecandy->update();
}

const Texture& LedCluster::getPatternTexture()
{
  return pattern_render.getTexture();;
}


void LedCluster::addStrip(glm::vec3 vertex_start, glm::vec3 vertex_end, int divisions) {

  glm::vec3 vertex_delta = vertex_end - vertex_start ;

  glm::vec2 texture_start;// = model.meshes[0].getVertex(start).TexCoords;
  glm::vec2 texture_end;//   = model.meshes[0].getVertex(end).TexCoords;

  glm::vec2 texture_delta = texture_end - texture_start;

  int width = leds_for_display.getDefaultTexture().width;
  int height = leds_for_display.getDefaultTexture().height;

  for(int i = 0;i < divisions;i++) {
    glm::vec3 ballPosDelta = vertex_start  + vertex_delta  * (1.0f/divisions)*float(i);
    glm::vec2 texDelta     = texture_start + texture_delta * (1.0f/divisions)*float(i);
    
    int count = numLeds();
    int x = count % width;
    int y = count / height;
    glm::vec3 planePosDelta((float)x + 0.5f, (float)y + 0.5f, 0.0f);

    LedVertex vertex_calc;
    vertex_calc.Position = ballPosDelta; 
    vertex_calc.TexCoords = texDelta;
    vertex_calc.framebuffer_proj = planePosDelta;

    leds_for_calc.addVertex(vertex_calc);

    // fprintf(stderr, "x: %3d, y: %3d\n", x, y);
    // fprintf(stderr, "x: %4.1f, y: %4.1f, z: %4.1f\n", ballPosDelta.x, ballPosDelta.y, ballPosDelta.z);

    leds_for_display.addInstance(ballPosDelta, glm::vec2(((float)x + 0.5) / width, ((float)y + 0.5) / height), glm::vec3());
  }
}


// LedCluster::Strip::Strip() :strip(0), strip_offset(0), offset(0), size(0) { }

// LedCluster::Strip::Strip(int strip, int strip_offset, int offset, int size) :
//   strip(strip), strip_offset(strip_offset), offset(offset), size(size) { }

// LedCluster::Strip::Strip(const Strip& copy) :
//   strip(copy.strip), strip_offset(copy.strip_offset), offset(copy.offset), size(copy.size) { }