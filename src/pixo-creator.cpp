#include <pixpq/pixpq.hpp>
#include <utility>

using namespace pixpq::sculpture;

int main( int argc, char** argv )
{
  bool reverse = false;
  if(argc < 3) {
    fprintf(stderr, "Usage: %s name LEDS_PER_SIDE [--reverse] [hostname]*\n", argv[0]);
    exit(1);
  }

  pixpq::manager manager("");
  manager.ensure_schema();

  int argc_i = 1;
  std::string sculpture_name(argv[argc_i++]);
  settings s = manager.get<settings>(settings::get_or_create(sculpture_name, "", 0.1, 1));

  const int leds_per_side = atoi(argv[argc_i++]);
  
  std::vector<std::pair<pixpq::sculpture::fadecandy, std::list<pixpq::sculpture::led>>> hosts;

  if (std::string("--reverse") == std::string(argv[argc_i])) {
    reverse = true;
    argc_i++;
  }

  for(;argc_i < argc;argc_i++) {
    hosts.push_back(
      std::make_pair(
        manager.get<fadecandy>(fadecandy::get_or_create(s, argv[argc_i])),
        std::list<led>()
      )
    );
  }


  float spacing = 0.04318;

  int width = leds_per_side;
  int height = leds_per_side;
  int depth = leds_per_side;

  float x_offset = -(float)width  / 2.0f  + 0.5f;
  float y_offset = -(float)height / 2.0f + 0.5f;
  float z_offset = -(float)depth  / 2.0f  - 0.5f;

  // camera_perspective.yaw = 108.0f;
  // camera_perspective.pitch = 12.0f;
  // camera_perspective.zoom = 2.4f;

  // projection_perspective.scope = glm::vec3(x_offset, y_offset, z_offset)*-spacing;
  // projection_perspective.yaw = 108.0f;
  // projection_perspective.pitch = 12.0f;
  // projection_perspective.zoom = 2.4f;

  int num_fadecandies = hosts.size();
  int per_fc = height / num_fadecandies;

  for(int y = height;y > 0;y--) {
    int direction = reverse ? 1 : -1;
    int selection = (y-1) / per_fc;

    for(int z = 0;z < depth;z++) {
      for(int x = std::max(-direction * (width - 1), 0); x >= 0 && x < width;x+=direction) {
        hosts[selection].second.push_back(led(((float)x + x_offset)*spacing,
                                               ((float)z + y_offset)*spacing,
                                               ((float)y + z_offset)*spacing));
      }
      direction *= -1;
    }
  }

  for (auto host : hosts) {
    manager.get_all<led>(led::upsert(host.first, host.second));
  }

  return 0;
}


