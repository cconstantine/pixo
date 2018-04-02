#include <pixlib/fade_candy.hpp>
#include <opengl.h>

namespace Pixlib {


  FadeCandy::FadeCandy(const std::string& hostname)
  {
    ALOGV("hostname: %s\n", hostname.c_str());
    opc_client.resolve(hostname.c_str());
  }

  void FadeCandy::addLed(const glm::vec3& position)
  {
    LedInfo li({position});
    leds.push_back(li);
  }


  void FadeCandy::finalize()
  { 
    int frameBytes = leds.size() * 3;
    framebuffer.resize(sizeof(OPCClient::Header) + frameBytes);

    OPCClient::Header::view(framebuffer).init(0, opc_client.SET_PIXEL_COLORS, frameBytes);
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
