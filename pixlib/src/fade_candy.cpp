#include <pixlib/fade_candy.hpp>
#include <opengl.h>

namespace Pixlib {


  FadeCandy::FadeCandy(const std::string& hostname) : hostname(hostname)
  {
    ALOGV("hostname: %s\n", hostname.c_str());
    opc_client.resolve(hostname.c_str());
  }

  FadeCandy::~FadeCandy()
  {
    ALOGV("clearing: %s\n", hostname.c_str());
    clear();
  }

  void FadeCandy::add_led(const glm::vec3& position)
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

  const std::vector<LedInfo>& FadeCandy::get_leds()
  {
    return leds;
  }

  void FadeCandy::update()
  {
    opc_client.write(framebuffer);
  }

  void FadeCandy::clear()
  {
    memset(get_data(), 0, get_leds().size()*3);
    update();
  }

  uint8_t* FadeCandy::get_data()
  {
    return (uint8_t*)OPCClient::Header::view(framebuffer).data();
  }
}
