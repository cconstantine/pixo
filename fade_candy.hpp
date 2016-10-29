#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <opc_client.h>

class FadeCandy {
public:
	FadeCandy(const std::string& hostname, unsigned int per_size);

	const std::vector<glm::vec3>& getLeds();
	void update();

	uint8_t* getData();
private:

	void add_strip(glm::vec3 start, glm::vec3 end, unsigned int length);
  void finalize();

	std::vector<glm::vec3> leds;
	std::vector<uint8_t> framebuffer;
	OPCClient opc_client;

};