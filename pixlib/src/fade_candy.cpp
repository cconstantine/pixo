#include <pixlib/fade_candy.hpp>
#include <opengl.h>

namespace Pixlib {

  FadeCandy::FadeCandy(const std::string& hostname, size_t size)
   : hostname(hostname), size(size),running(true), writer_thread(&FadeCandy::thread_method, this)
  {
    //ALOGV("connecting to: %s\n", hostname.c_str());
    //opc_client.resolve(hostname.c_str());

    int frameBytes = size * 3;
    framebuffer.resize(sizeof(OPCClient::Header) + frameBytes);

    OPCClient::Header::view(framebuffer).init(0, opc_client.SET_PIXEL_COLORS, frameBytes);
  }

  FadeCandy::~FadeCandy()
  {
    ALOGV("clearing: %s\n", hostname.c_str());
    running = false;
    clear();
    writer_thread.join();
  }

  void FadeCandy::update()
  {
    std::unique_lock<std::mutex> lck(pending_mutex);

    pending_message.notify_all();
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

  void FadeCandy::thread_method()
  {
    bool was_connected = true;
    while(running) {
      {
        std::unique_lock<std::mutex> lck(pending_mutex);
        pending_message.wait(lck);
      }

      // fprintf(stderr, "%s: thread (%d)!\n", hostname.c_str(), opc_client.isConnected());
      if(running || opc_client.isConnected()) {
        if (was_connected && !opc_client.isConnected()) {
            fprintf(stderr, "Connecting to: %s\n", hostname.c_str());
        }
        was_connected = opc_client.isConnected();
        opc_client.write(hostname.c_str(), framebuffer);
      }
    }
  }
}
