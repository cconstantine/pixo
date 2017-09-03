#include <led_cluster.hpp>
#include <math.h>

using namespace std;

size_t LedCluster::led_canvas_size(size_t leds)
{
  fprintf(stderr, "leds: %d\n", leds);
  return pow(2, ceil(log(sqrt(leds))/log(2)));
}


LedCluster::LedCluster(FadeCandy *fadecandy) :
 led_texture(led_canvas_size(fadecandy->getLeds().size()), led_canvas_size(fadecandy->getLeds().size())),
 leds_for_calc(pattern_render.getTexture()),
 leds_for_display(led_texture),
 fb_render(led_texture),
 pattern_render(fadecandy->textureSize()),
 fadecandy(fadecandy),
 render_timer(120)
{
  //leds_for_calc.addTexture(pattern_render.getDepthTexture());
  int width = leds_for_display.getDefaultTexture().width;
  int height = leds_for_display.getDefaultTexture().height;
  fprintf(stderr, "Led canvas: %4d x %4d (total: %d)\n", width, height, width*height);

  for(int i = 0;i < this->fadecandy->getLeds().size();i++) {

    LedInfo led_info = this->fadecandy->getLeds()[i];
    glm::vec3 ballPosDelta = led_info.position;
    int count = numLeds();
    int x = count % width;
    int y = count / height;
    glm::vec3 planePosDelta((float)x + 0.5f, (float)y + 0.5f, 0.0f);

    LedVertex vertex_calc;
    vertex_calc.Position = ballPosDelta;
    vertex_calc.TexCoords = led_info.texture_coordinates;
    vertex_calc.framebuffer_proj = planePosDelta;

    leds_for_calc.addVertex(vertex_calc);

    leds_for_display.addInstance(ballPosDelta, glm::vec2(((float)x + 0.5) / width, ((float)y + 0.5) / height), glm::vec3());
  }


  leds_for_calc.setupMesh();
  fb_render.leds = &leds_for_calc;
}

GLuint LedCluster::numLeds() {
  return leds_for_calc.numVertices();
}

void LedCluster::Draw(Shader shader) 
{
  leds_for_display.Draw(shader);
}

void LedCluster::render(const IsoCamera& viewed_from, const Pattern& pattern) 
{
  render_timer.start();
  pattern_render.render(pattern);
  fb_render.render(viewed_from, fadecandy->getData(), numLeds()*3);
  render_timer.end();

  fadecandy->update();
}

float LedCluster::render_time()
{
  return render_timer.duration();
}

const Texture& LedCluster::getPatternTexture()
{
  return pattern_render.getTexture();
}
