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

  class Sculpture {
  public:
    static Sculpture load(const std::string& filename);

    Sculpture();
    Sculpture(int id, const std::vector<std::string>& fadecandies, unsigned int per_size);
    void save(const std::string& filename);

    int id;
    Point scope;
    LedGeometries leds;
    int canvas_width, canvas_height;
  };

  class PatternCode {
  public:
    static std::vector<PatternCode> load_all(const std::string& db_filename);
    static void upsert(const std::string& db_filename, const std::vector<std::string>& filenames);

    PatternCode();
    PatternCode(const std::string& name, const std::string& shader_code);

    int id;
    std::string name;
    std::string shader_code;
    bool enabled;
  };
}
