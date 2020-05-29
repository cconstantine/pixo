#include <fstream>

#include <pixlib/data_model.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Pixlib {
  LedGeometry::LedGeometry() : locations(std::make_shared<std::vector<Point>>()) {}
  LedGeometry::LedGeometry(const std::string& host) : fadecandy_host(host), locations(std::make_shared<std::vector<Point>>()) {}

  Perspective::Perspective() : id(0) {}

  Sculpture::Sculpture() {}
  Sculpture::Sculpture(int id, const std::vector<std::string>& fadecandies, unsigned int per_size) :
      canvas_width(per_size*per_size), canvas_height(per_size*per_size), id(id), brightness(1.0), rotation(0.0)
  {
    for(std::string host : fadecandies) {
      leds.push_back(LedGeometry(host));
    }
    float spacing = 0.04318;

    int width = per_size;
    int height = per_size;
    int depth = per_size;

    float x_offset = -(float)width  / 2.0f  + 0.5f;
    float y_offset = -(float)height / 2.0f - 0.5f;
    float z_offset = -(float)depth  / 2.0f  + 0.5f;

    camera_perspective.yaw = 108.0f;
    camera_perspective.pitch = 12.0f;
    camera_perspective.zoom = 2.4f;

    projection_perspective.scope = glm::vec3(x_offset, y_offset, z_offset)*-spacing;
    projection_perspective.yaw = 108.0f;
    projection_perspective.pitch = 12.0f;
    projection_perspective.zoom = 2.4f;

    int num_fadecandies = fadecandies.size();
    int per_fc = height / num_fadecandies;

    for(int y = height;y > 0;y--) {
      int direction = -1;
      int selection = (y-1) / per_fc;

      LedGeometry& geom = leds[selection];

      for(int z = 0;z < height;z++) {
        for(int x = std::max(-direction * (width - 1), 0); x >= 0 && x < width;x+=direction) {
          geom.locations->push_back(glm::vec3( ((float)x + x_offset)*spacing,
                                               ((float)z + z_offset)*spacing,
                                               ((float)y + y_offset)*spacing));
        }
        direction *= -1;
      }
    }
  }

  TrackingService::TrackingService() {}

  PatternCode::PatternCode() {}

  PatternCode::PatternCode(const std::string& name, const std::string& shader_code)
   : id(-1), name(name), shader_code(shader_code), enabled(true)
  { }



}