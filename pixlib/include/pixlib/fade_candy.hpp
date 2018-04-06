#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <opc_client.h>


namespace Pixlib {
  struct LedInfo {
    // Position
    glm::vec3 position;
  };

  class FadeCandy {
  public:
    FadeCandy(const std::string& hostname);
    ~FadeCandy();

    void add_led(const glm::vec3& position);

    void finalize();

    const std::vector<LedInfo>& get_leds();
  	void update();
    void clear();

    uint8_t* get_data();
  private:

    std::string hostname;
  	std::vector<LedInfo> leds;
  	std::vector<uint8_t> framebuffer;
  	OPCClient opc_client;

  };
}
