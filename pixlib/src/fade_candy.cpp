#include <pixlib/fade_candy.hpp>


namespace Pixlib {
  FadeCandy::FadeCandy(const std::string& hostname, unsigned int per_size)
  {
    opc_client.resolve(hostname.c_str());
    float spacing = 43.18 / 1000.0f;

    int width = per_size;
    int height = per_size;
    int depth = per_size;

    float x_offset = -(float)width / 2.0f  + 0.5f;
    float y_offset = -(float)height / 2.0f - 0.5f;
    float z_offset = -(float)depth / 2.0f  + 0.5f;

    for(int y = height;y > 0;y--) {
      int direction = 1;
      for(int z = 0;z < height;z++) {
        for(int x = std::max(-direction * (width - 1), 0); x >= 0 && x < width;x+=direction) {
          LedInfo li;
          li.position = glm::vec3(((float)x + x_offset)*spacing, ((float)z+z_offset)*spacing, ((float)y+y_offset)*spacing);
          
          leds.push_back(li);
        }  
        
        direction *= -1;
      }
    }

    finalize();
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

  const std::vector<LedInfo>& FadeCandy::getLeds()
  {
    return leds;
  }

  void FadeCandy::update()
  {
    opc_client.write(framebuffer);
  }

  void FadeCandy::clear()
  {
    memset(getData(), 0, getLeds().size()*3);
    update();
  }

  uint8_t* FadeCandy::getData()
  {
    return (uint8_t*)OPCClient::Header::view(framebuffer).data();
  }
}
