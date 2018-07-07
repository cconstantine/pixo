#include <fstream>

#include <pixlib/data_model.hpp>


#include <glm/gtx/string_cast.hpp>
using namespace sqlite_orm;

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

            Pixlib::Point li(x, y, z);
            return li;
        }
    };
}

namespace Pixlib {
  LedGeometry::LedGeometry() : locations(std::make_shared<std::vector<Point>>()) {}
  LedGeometry::LedGeometry(const std::string& host) : fadecandy_host(host), locations(std::make_shared<std::vector<Point>>()) {}

  Perspective::Perspective() : id(0) {}

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


  Storage::Storage(const std::string& filename, const Sculpture& sculpture)
   :  storage(init_storage(filename)), sculpture(sculpture)
  {
    storage.sync_schema();
    save();
  }

  Storage::Storage(const std::string& filename) : storage(init_storage(filename)), sculpture(load_scupture())
  {
    storage.sync_schema();
  }

  Sculpture Storage::load_scupture() {
    Sculpture sculpture = storage.get<Sculpture>(1);
    sculpture.leds = storage.get_all<LedGeometry>();

    sculpture.camera_perspective = storage.get<Perspective>(sculpture.camera_perspective_id);
    sculpture.projection_perspective = storage.get<Perspective>(sculpture.projection_perspective_id);

    return sculpture;
  }

  void Storage::save() {
    storage.remove_all<Pixlib::Sculpture>();
    storage.remove_all<Pixlib::LedGeometry>();
    storage.remove_all<Pixlib::Perspective>();

    sculpture.camera_perspective.id = storage.insert<Pixlib::Perspective>(sculpture.camera_perspective);
    sculpture.projection_perspective.id = storage.insert<Pixlib::Perspective>(sculpture.projection_perspective);

    sculpture.camera_perspective_id = sculpture.camera_perspective.id;
    sculpture.projection_perspective_id = sculpture.projection_perspective.id;

    storage.insert<Pixlib::Sculpture>(sculpture);

    for(const LedGeometry& geom : sculpture.leds) {
      storage.insert<LedGeometry>(geom);
    }
  }

  std::vector<PatternCode> Storage::patterns() {
    return storage.get_all<Pixlib::PatternCode>();
  }

  void Storage::upsert_patterns(const std::vector<std::string>& filenames) {
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