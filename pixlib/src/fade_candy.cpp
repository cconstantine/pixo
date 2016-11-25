#include <fade_candy.hpp>


FadeCandy::FadeCandy(const std::string& hostname, unsigned int per_size)
{
  opc_client.resolve(hostname.c_str());
  float spacing = .3;

  int width = per_size;
  int height = per_size;
  int depth = per_size;
  for(int i = 0;i < width;i++) {
    for(int j = 0;j < depth;j++) {
      glm::vec3 start((i - width/2)*spacing, -(height/2)*spacing,      (j - height/2)*spacing);
      glm::vec3 end  ((i - width/2)*spacing, (height/2+1)*spacing, (j - height/2)*spacing);


      add_strip(start, end, height);
    }
  }
  finalize();
}


void FadeCandy::add_strip(glm::vec3 start, glm::vec3 end, unsigned int divisions)
{
  glm::vec3 delta = end - start ;

  for(int i = 0;i < divisions;i++) {
    glm::vec3 pos = start  + delta  * (1.0f/divisions)*float(i);
    
    leds.push_back(pos);
  }
}

void FadeCandy::finalize()
{ 
  int frameBytes = leds.size() * 3;
  framebuffer.resize(sizeof(OPCClient::Header) + frameBytes);

  OPCClient::Header::view(framebuffer).init(0, opc_client.SET_PIXEL_COLORS, frameBytes);
}

glm::vec2 FadeCandy::textureSize()
{
  int canvasSize = sqrt(getLeds().size())*2;
  return glm::vec2(canvasSize, canvasSize);
}

const std::vector<glm::vec3>& FadeCandy::getLeds()
{
  return leds;
}

void FadeCandy::update()
{
  opc_client.write(framebuffer);
}

uint8_t* FadeCandy::getData()
{
  return (uint8_t*)OPCClient::Header::view(framebuffer).data();
}
