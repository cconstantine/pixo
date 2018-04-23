#include <fstream>

#include <pixlib/data_model.hpp>


#include <sqlite_orm/sqlite_orm.h>
#include <glm/gtx/string_cast.hpp>

namespace sqlite_orm {

    template<>
    struct type_printer<Pixlib::PointSpace> : public text_printer {};

    template<>
    struct statement_binder<Pixlib::PointSpace> {

        int bind(sqlite3_stmt *stmt, int index, const Pixlib::PointSpace& value) {
            std::string s_value;
            {
              std::ostringstream stream;
              for(Pixlib::Point point : *value.get()) {
                char buff[265];
                snprintf(buff, sizeof(buff), "v %f %f %f\n", point.x, point.y, point.z);
                stream << std::string(buff);
              }
              s_value = stream.str();
            }
            return statement_binder<std::string>().bind(stmt, index,  s_value);
        }
    };

    template<>
    struct field_printer<Pixlib::PointSpace> {
        std::string operator()(const Pixlib::PointSpace &value) const {
            std::string s_value;
            {
              std::ostringstream stream;
              for(Pixlib::Point point : *value.get()) {
                char buff[265];
                snprintf(buff, sizeof(buff), "v %f %f %f\n", point.x, point.y, point.z);
                stream << std::string(buff);
              }
              s_value = stream.str();
            }
            return std::string("");
        }
    };

    template<>
    struct row_extractor<Pixlib::PointSpace> {
        Pixlib::PointSpace extract(sqlite3_stmt *stmt, int columnIndex) {
            Pixlib::PointSpace leds(std::make_shared<std::vector<Pixlib::Point>>());

            std::string s_value = row_extractor<std::string>().extract(stmt, columnIndex);//(reinterpret_cast<const char*>(sqlite3_column_text(stmt, columnIndex)));
            std::istringstream iss(s_value);

            for (std::string line; std::getline(iss, line); ) {
              float x, y, z = 0.0f;
              sscanf(line.c_str(), "v %f %f %f" , &x, &y, &z);

              leds->push_back(Pixlib::Point({glm::vec3(x,y,z)}));
            }
            return leds;
        }
    };

    template<>
    struct type_printer<Pixlib::Point> : public text_printer {};

    template<>
    struct statement_binder<Pixlib::Point> {

        int bind(sqlite3_stmt *stmt, int index, const Pixlib::Point& point) {
          std::string s_value;
          char buff[265];
          snprintf(buff, sizeof(buff), "v %f %f %f\n", point.x, point.y, point.z);
          return statement_binder<std::string>().bind(stmt, index,  std::string(buff));
        }
    };

    template<>
    struct field_printer<Pixlib::Point> {
        std::string operator()(const Pixlib::Point &point) const {
          std::string s_value;
          char buff[265];
          snprintf(buff, sizeof(buff), "v %f %f %f\n", point.x, point.y, point.z);
          return std::string(buff);
        }
    };

    template<>
    struct row_extractor<Pixlib::Point> {
        Pixlib::Point extract(sqlite3_stmt *stmt, int columnIndex) {
            Pixlib::Point value;
            std::string s_value = row_extractor<std::string>().extract(stmt, columnIndex);

            float x, y, z = 0.0f;
            sscanf(s_value.c_str(), "v %f %f %f" , &x, &y, &z);

            Pixlib::Point li((x, y, z));
            return li;
        }
    };
}

using namespace sqlite_orm;
namespace Pixlib {
  inline auto initStorage(const std::string &path) {
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
  using Storage = decltype(initStorage(""));

  LedGeometry::LedGeometry() : locations(std::make_shared<std::vector<Point>>()) {}
  LedGeometry::LedGeometry(const std::string& host) : fadecandy_host(host), locations(std::make_shared<std::vector<Point>>()) {}

  Sculpture Sculpture::load(const std::string& filename) {
    Storage storage = initStorage(filename.c_str());

    Sculpture sculpture = storage.get<Sculpture>(1);
    sculpture.leds = storage.get_all<LedGeometry>();

    return sculpture;
  }

  void Sculpture::save(const std::string& filename) {

    Pixlib::Storage storage = Pixlib::initStorage(filename.c_str());
    storage.sync_schema();
    storage.remove_all<Pixlib::Sculpture>();
    storage.remove_all<Pixlib::LedGeometry>();

    storage.insert<Pixlib::Sculpture>(*this);

    for(LedGeometry& geom : leds) {
      storage.insert<LedGeometry>(geom);
    }
  }

  Sculpture::Sculpture() {}
  Sculpture::Sculpture(int id, const std::vector<std::string>& fadecandies, unsigned int per_size) :
      canvas_width(per_size*per_size), canvas_height(per_size*per_size), id(id)
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

    scope = glm::vec3(x_offset, y_offset, z_offset)*-spacing;

    int num_fadecandies = fadecandies.size();
    int per_fc = height / num_fadecandies;

    for(int y = height;y > 0;y--) {
      int direction = 1;
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


  PatternCode::PatternCode() {}

  PatternCode::PatternCode(const std::string& name, const std::string& shader_code)
   : id(-1), name(name), shader_code(shader_code), enabled(true)
  { }

  struct MatchPathSeparator
  {
    bool operator()( const char ch ) const {
      return ch == '/';
    }
  };

  std::string basename( std::string const& pathname ) {
    return std::string(
      std::find_if( pathname.rbegin(), pathname.rend(),
                    MatchPathSeparator() ).base(),
      pathname.end() );
  }

  std::vector<PatternCode> PatternCode::load_all(const std::string& db_filename) {
    Pixlib::Storage storage = Pixlib::initStorage(db_filename.c_str());
    storage.sync_schema();

    return storage.get_all<Pixlib::PatternCode>();
  }

  void PatternCode::upsert(const std::string& db_filename, const std::vector<std::string>& filenames) {
    Pixlib::Storage storage = Pixlib::initStorage(db_filename.c_str());
    storage.sync_schema();
    storage.begin_transaction();

    for(const std::string& filename : filenames) {
      std::string pattern_name = basename(filename);

      std::ifstream t(filename);
      std::string code((std::istreambuf_iterator<char>(t)),
                        std::istreambuf_iterator<char>());

      std::string name_check = pattern_name;

      std::vector<Pixlib::PatternCode> patterns = storage.get_all<Pixlib::PatternCode>(where(c(&Pixlib::PatternCode::name) == pattern_name));
      if (patterns.size() == 0) {
        fprintf(stderr, "Inserting %s\n", pattern_name.c_str());

        storage.insert(Pixlib::PatternCode(pattern_name, code));
      }
      else {
        fprintf(stderr, "Updating %s\n", pattern_name.c_str());
        
        for(Pixlib::PatternCode pattern : patterns) {
          pattern.shader_code = code;
          storage.update(pattern);
        }
      }
    }
    storage.commit();
  }
}