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

  class Sculpture {
  public:

    Sculpture();
    Sculpture(int id, const std::vector<std::string>& fadecandies, unsigned int per_size);

    int id;
    Point scope;
    LedGeometries leds;
    int canvas_width, canvas_height;
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
                                     make_column("scope",
                                                 &Sculpture::scope),
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

    Sculpture sculpture;
  };
}
