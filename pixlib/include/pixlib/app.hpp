#pragma once
#include <vector>
#include <map>
#include <memory>

#include <pixlib/pattern.hpp>
#include <pixlib/camera.hpp>
#include <pixlib/led_cluster.hpp>
#include <pixlib/scene.hpp>
#include <pixlib/timer.hpp>
#include <pixlib/fade_candy.hpp>

#include <sqlite_orm/sqlite_orm.h>
#include <sstream>
#include <glm/gtx/string_cast.hpp>

namespace sqlite_orm {

    template<>
    struct type_printer<std::vector<Pixlib::LedInfo>> : public text_printer {};

    template<>
    struct statement_binder<std::vector<Pixlib::LedInfo>> {

        int bind(sqlite3_stmt *stmt, int index, const std::vector<Pixlib::LedInfo>& value) {
            std::string s_value;
            {
              std::ostringstream stream;
              for(Pixlib::LedInfo info : value) {
                char buff[265];
                snprintf(buff, sizeof(buff), "v %f %f %f\n", info.position.x, info.position.y, info.position.z);
                stream << std::string(buff);
              }
              s_value = stream.str();
            }
            return statement_binder<std::string>().bind(stmt, index,  s_value);
        }
    };

    /**
     *  field_printer is used in `dump` and `where` functions. Here we have to create
     *  a string from mapped object.
     */
    template<>
    struct field_printer<std::vector<Pixlib::LedInfo>> {
        std::string operator()(const std::vector<Pixlib::LedInfo> &t) const {
            std::string s_value;
            {
              std::ostringstream stream;
              for(Pixlib::LedInfo info : t) {
                char buff[265];
                snprintf(buff, sizeof(buff), "v %f %f %f\n", info.position.x, info.position.y, info.position.z);
                stream << std::string(buff);
              }
              s_value = stream.str();
            }
            return std::string("");
        }
    };

    /**
     *  This is a reverse operation: here we have to specify a way to transform string received from
     *  database to our Gender object. Here we call `GenderFromString` and throw `std::runtime_error` if it returns
     *  nullptr. Every `row_extractor` specialization must have `extract(const char*)` and `extract(sqlite3_stmt *stmt, int columnIndex)`
     *  functions which return a mapped type value.
     */
    template<>
    struct row_extractor<std::vector<Pixlib::LedInfo>> {
        std::vector<Pixlib::LedInfo> extract(sqlite3_stmt *stmt, int columnIndex) {
            std::vector<Pixlib::LedInfo> leds;
            std::string s_value = row_extractor<std::string>().extract(stmt, columnIndex);//(reinterpret_cast<const char*>(sqlite3_column_text(stmt, columnIndex)));
            std::istringstream iss(s_value);


            for (std::string line; std::getline(iss, line); ) {
              float x, y, z = 0.0f;
              sscanf(line.c_str(), "v %f %f %f" , &x, &y, &z);

              Pixlib::LedInfo li({glm::vec3(x, y, z)});
              leds.push_back(li);
            }
            return leds;
        }
    };
}


namespace Pixlib {

  typedef  std::vector<std::shared_ptr<LedCluster>> LedClusterCluster;


  inline auto initStorage(const std::string &path) {
    using namespace sqlite_orm;
    return make_storage("db.sqlite",
                          make_table("fadecandies",
                                     make_column("id",
                                                 &FadeCandy::id,
                                                 autoincrement(),
                                                 primary_key()),
                                     // make_column("hostname",
                                     //             &FadeCandy::get_hostname,
                                     //             &FadeCandy::set_hostname),
                                     make_column("leds",
                                                 &FadeCandy::leds)
                                     )
                          );
  }
  using Storage = decltype(initStorage(""));

  class App {
  public:

    App(const std::string& db_filename, glm::vec2 canvas_size);

    static void BuildPixo(FadeCandyCluster& fadecandies, unsigned int per_size);
    void add_fadecandy(const FadeCandyCluster& fadecandies);
    
    float scene_fps();
    float scene_render_time();
    float led_render_time();

    const Texture& get_pattern_texture();

    void process_mouse_movement(int xoffset, int yoffset);
    void process_mouse_scroll(float x);

    void move_perspective_to_camera();
    void set_screen_size(int width, int height);

    void register_pattern(std::string name, std::shared_ptr<Pattern> pattern);

    void set_random_pattern();

    const std::string& get_pattern();

    void tick();

    float brightness;
    Storage storage;

  private:
    std::map<std::string, std::shared_ptr<Pattern> > patterns;
    std::string pattern_name;

    Scene scene;
    PatternRender pattern_render;

    LedClusterCluster led_clusters;

    IsoCamera viewed_from;
    IsoCamera camera;

  };
}
