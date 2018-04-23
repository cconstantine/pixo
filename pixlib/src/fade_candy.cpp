#include <pixlib/fade_candy.hpp>
#include <opengl.h>

namespace Pixlib {

  FadeCandy::FadeCandy(const std::string& hostname, size_t size) : hostname(hostname), size(size)
  {
    ALOGV("connecting to: %s\n", hostname.c_str());
    opc_client.resolve(hostname.c_str());

    int frameBytes = size * 3;
    framebuffer.resize(sizeof(OPCClient::Header) + frameBytes);

    OPCClient::Header::view(framebuffer).init(0, opc_client.SET_PIXEL_COLORS, frameBytes);
  }

  FadeCandy::~FadeCandy()
  {
    ALOGV("clearing: %s\n", hostname.c_str());
    clear();
  }

  void FadeCandy::update()
  {
    opc_client.write(framebuffer);
  }

  void FadeCandy::clear()
  {
    memset(get_data(), 0, size*3);
    update();
  }

  uint8_t* FadeCandy::get_data()
  {
    return (uint8_t*)OPCClient::Header::view(framebuffer).data();
  }
}
