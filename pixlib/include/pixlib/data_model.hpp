#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include <sqlite_orm/sqlite_orm.h>

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

  using namespace sqlite_orm;
  inline auto init_storage(const std::string &path) {
    using namespace sqlite_orm;
    return make_storage(path.c_str(),
                          make_table("perspectives",
                                     make_column("id",
                                                 &Perspective::id,
                                                 autoincrement(),
                                                 primary_key()),
                                     make_column("scope",
                                                 &Perspective::scope),
                                     make_column("yaw",
                                                 &Perspective::yaw),
                                     make_column("pitch",
                                                 &Perspective::pitch),
                                     make_column("zoom",
                                                 &Perspective::zoom)
                                     ),
                          make_table("led_geometries",
                                     make_column("id",
                                                 &LedGeometry::id,
                                                 autoincrement(),
                                                 primary_key()),
                                     make_column("hostname",
                                                 &LedGeometry::fadecandy_host),
                                     make_column("locations",
                                                 &LedGeometry::locations)
                                     ),
                          make_table("sculptures",
                                     make_column("id",
                                                 &Sculpture::id,
                                                 primary_key()),
                                     make_column("camera_perspective_id",
                                                 &Sculpture::camera_perspective_id),
                                     make_column("projection_perspective_id",
                                                 &Sculpture::projection_perspective_id),
                                     make_column("active_pattern_name",
                                                 &Sculpture::active_pattern_name ),
                                     make_column("brightness",
                                                 &Sculpture::brightness ),
                                     make_column("canvas_width",
                                                 &Sculpture::canvas_width),
                                     make_column("canvas_height",
                                                 &Sculpture::canvas_height)

                                     ),
                          make_table("pattern_codes",
                                     make_column("id",
                                                 &PatternCode::id,
                                                 autoincrement(),
                                                 primary_key()),
                                     make_column("name",
                                                 &PatternCode::name,
                                                 unique()),
                                     make_column("shader_code",
                                                 &PatternCode::shader_code),
                                     make_column("enabled",
                                                 &PatternCode::enabled)
                                     )
                          );
  }
  using SqliteStorage = decltype(init_storage(""));

  class Storage {
  private:
    Sculpture load_scupture();
    void      save();

    SqliteStorage storage;

  public:
    Storage(const std::string& filename,  const Sculpture& Sculpture);
    Storage(const std::string& filename);

    std::vector<PatternCode> patterns();

    void upsert_patterns(const std::vector<std::string>& filenames);
    void      save_app_state();

    Sculpture sculpture;
  };
}
