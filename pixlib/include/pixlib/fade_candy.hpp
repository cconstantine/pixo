#pragma once

#include <string>
#include <vector>
#include <memory>

#include <glm/glm.hpp>
#include <opc_client.h>


namespace Pixlib {

  struct LedInfo {
    // Position
    glm::vec3 position;
  };

  class FadeCandy {
  public:
    int id;
    std::string hostname;
    std::vector<LedInfo> leds;

    FadeCandy();
    FadeCandy(const std::string& hostname);
    ~FadeCandy();

    void add_led(const glm::vec3& position);

    void set_hostname(const std::string& hostname);
    const std::string& get_hostname();

    void finalize();

  	void update();
    void clear();

    uint8_t* get_data();
  private:
  	std::vector<uint8_t> framebuffer;
  	OPCClient opc_client;

  };

  class FadeCandyCluster : public std::vector<std::shared_ptr<FadeCandy>> {
  public:
    glm::vec3 scope;
  };

}
