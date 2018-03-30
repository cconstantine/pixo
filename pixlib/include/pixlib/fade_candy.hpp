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
  	FadeCandy(const std::string& hostname, unsigned int num_leds);

  	const std::vector<LedInfo>& getLeds();
  	glm::vec2 textureSize();
  	void update();
    void clear();

  	uint8_t* getData();
  private:

    	void finalize();

  	std::vector<LedInfo> leds;
  	std::vector<uint8_t> framebuffer;
  	OPCClient opc_client;

  };
}
