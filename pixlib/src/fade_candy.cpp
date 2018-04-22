#include <pixlib/fade_candy.hpp>
#include <opengl.h>

namespace Pixlib {

  FadeCandy::FadeCandy() {
    ALOGV("Create fc %p\n", this);
  }

  FadeCandy::FadeCandy(const std::string& hostname) : hostname(hostname)
  {
    ALOGV("hostname: %s\n", hostname.c_str());
    opc_client.resolve(hostname.c_str());
  }

  FadeCandy::~FadeCandy()
  {
    ALOGV("size: %d\n", leds.size());
    ALOGV("Destroy fc %p\n", this);
    //clear();
  }


  void FadeCandy::set_hostname(const std::string& hostname) {
    ALOGV("hostname: %s\n", hostname.c_str());

    this->hostname = hostname;
    opc_client.resolve(hostname.c_str());
  }

  const std::string& FadeCandy::get_hostname() {
    return hostname;
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


  void FadeCandy::update()
  {
    opc_client.write(framebuffer);
  }

  void FadeCandy::clear()
  {
    memset(get_data(), 0, leds.size()*3);
    update();
  }

  uint8_t* FadeCandy::get_data()
  {
    return (uint8_t*)OPCClient::Header::view(framebuffer).data();
  }
}
