#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>


namespace Pixlib {
  typedef glm::vec3 Point;
  typedef std::shared_ptr<std::vector<Point>> PointSpace;

  class LedGeometry {
    public:
      LedGeometry();
      LedGeometry(const std::string& fadecandy_host);
      int id;
      std::string fadecandy_host;
      PointSpace locations;
  };

  typedef std::vector<LedGeometry> LedGeometries;

  class Perspective {
  public:
    Perspective();

    int id;
    Point scope;
    double yaw, pitch, zoom;
  };

  class Sculpture {
  public:

    Sculpture();
    Sculpture(int id, const std::vector<std::string>& fadecandies, unsigned int per_size);

    int id;
    int camera_perspective_id;
    int projection_perspective_id;

    LedGeometries leds;
    int canvas_width, canvas_height;

    Perspective camera_perspective;
    Perspective projection_perspective;

    std::string active_pattern_name;
    double brightness;
    double rotation;
  };

  class PatternCode {
  public:
    PatternCode();
    PatternCode(const std::string& name, const std::string& shader_code);

    int id;
    std::string name;
    std::string shader_code;
    bool enabled;
  };

}
