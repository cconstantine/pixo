#include <storage.hpp>

using namespace Pixlib;
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
  storage.begin_transaction();

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
  storage.commit();
}

void Storage::save_app_state()
{
  storage.begin_transaction();
  storage.update<Pixlib::Perspective>(sculpture.camera_perspective);
  storage.update<Pixlib::Perspective>(sculpture.projection_perspective);
  storage.update<Pixlib::Sculpture>(sculpture);
  storage.commit();
}

std::vector<TrackingService> Storage::tracking_services() {
  return storage.get_all<Pixlib::TrackingService>();
}

std::vector<PatternCode> Storage::patterns() {
  return storage.get_all<Pixlib::PatternCode>(where(c(&PatternCode::enabled) == true));
}

void Storage::upsert_tracker_service(const std::string& addr, const Pixlib::Point& offset) {
  storage.begin_transaction();

  std::vector<Pixlib::TrackingService> services = storage.get_all<Pixlib::TrackingService>(where(c(&Pixlib::TrackingService::address) == addr));

  if(services.size() == 0) {
    printf("Inserting %s\n", addr.c_str());

    TrackingService ts;
    ts.address = addr;
    ts.tracking_offset = offset;

    storage.insert(ts);
  } else {
    printf("Updating %s\n", addr.c_str());

    for(Pixlib::TrackingService service : services) {
      service.tracking_offset = offset;

      storage.update(service);
    }
  }
  storage.commit();
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
      printf("Inserting %s\n", pattern_name.c_str());

      storage.insert(Pixlib::PatternCode(pattern_name, code));
    }
    else {
      printf("Updating %s\n", pattern_name.c_str());
      
      for(Pixlib::PatternCode pattern : patterns) {
        pattern.shader_code = code;
        storage.update(pattern);
      }
    }
  }
  storage.commit();
}