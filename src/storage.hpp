#include <sqlite_orm/sqlite_orm.h>
#include <pixlib.hpp>
#include <pixlib/data_model.hpp>

using namespace sqlite_orm;
inline auto init_storage(const std::string &path) {
  using namespace sqlite_orm;
  return make_storage(path.c_str(),
                        make_table("perspectives",
                                   make_column("id",
                                               &Pixlib::Perspective::id,
                                               autoincrement(),
                                               primary_key()),
                                   make_column("scope",
                                               &Pixlib::Perspective::scope),
                                   make_column("yaw",
                                               &Pixlib::Perspective::yaw),
                                   make_column("pitch",
                                               &Pixlib::Perspective::pitch),
                                   make_column("zoom",
                                               &Pixlib::Perspective::zoom)
                                   ),
                        make_table("led_geometries",
                                   make_column("id",
                                               &Pixlib::LedGeometry::id,
                                               autoincrement(),
                                               primary_key()),
                                   make_column("hostname",
                                               &Pixlib::LedGeometry::fadecandy_host),
                                   make_column("locations",
                                               &Pixlib::LedGeometry::locations)
                                   ),
                        make_table("sculptures",
                                   make_column("id",
                                               &Pixlib::Sculpture::id,
                                               primary_key()),
                                   make_column("camera_perspective_id",
                                               &Pixlib::Sculpture::camera_perspective_id),
                                   make_column("projection_perspective_id",
                                               &Pixlib::Sculpture::projection_perspective_id),
                                   make_column("active_pattern_name",
                                               &Pixlib::Sculpture::active_pattern_name ),
                                   make_column("brightness",
                                               &Pixlib::Sculpture::brightness ),
                                   make_column("rotation",
                                               &Pixlib::Sculpture::rotation ),
                                   make_column("canvas_width",
                                               &Pixlib::Sculpture::canvas_width),
                                   make_column("canvas_height",
                                               &Pixlib::Sculpture::canvas_height)

                                   ),
                        make_table("pattern_codes",
                                   make_column("id",
                                               &Pixlib::PatternCode::id,
                                               autoincrement(),
                                               primary_key()),
                                   make_column("name",
                                               &Pixlib::PatternCode::name,
                                               unique()),
                                   make_column("shader_code",
                                               &Pixlib::PatternCode::shader_code),
                                   make_column("enabled",
                                               &Pixlib::PatternCode::enabled)
                                   ),
                        make_table("tracking_services",
                                   make_column("id",
                                               &Pixlib::TrackingService::id,
                                               autoincrement(),
                                               primary_key()),
                                   make_column("address",
                                               &Pixlib::TrackingService::address,
                                               unique()),
                                   make_column("tracking_offset",
                                               &Pixlib::TrackingService::tracking_offset)
                                   )
                        );
}
using SqliteStorage = decltype(init_storage(""));

class Storage {
private:
  Pixlib::Sculpture load_scupture();
  void      save();

  SqliteStorage storage;

public:
  Storage(const std::string& filename,  const Pixlib::Sculpture& Sculpture);
  Storage(const std::string& filename);

  std::vector<Pixlib::TrackingService> tracking_services();
  void upsert_tracker_service(const std::string& addr, const Pixlib::Point& offset);

  std::vector<Pixlib::PatternCode> patterns();
  void upsert_patterns(const std::vector<std::string>& filenames);
  void      save_app_state();

  Pixlib::Sculpture sculpture;
};
