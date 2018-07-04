#pragma once

#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <condition_variable>

#include <glm/glm.hpp>
#include <opc_client.h>


namespace Pixlib {

  struct LedInfo {
    // Position
    glm::vec3 position;
  };

  class FadeCandy {
  public:

    FadeCandy(const std::string& hostname, size_t size);
    ~FadeCandy();

  	void update();
    void clear();

    uint8_t* get_data();
  private:
    size_t size;
    std::string hostname;
  	std::vector<uint8_t> framebuffer;
  	OPCClient opc_client;

    std::condition_variable pending_message;
    std::mutex pending_mutex;
    std::thread writer_thread;
    bool running;
    void thread_method();
  };
}
