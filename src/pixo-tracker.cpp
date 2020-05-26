#include <storage.hpp>
#include <glm/gtx/string_cast.hpp>

std::string Pixlib::Shader::ShaderPreamble = "#version 330\n";


int main( int argc, char** argv )
{  
  if(argc < 5) {
    fprintf(stderr, "Usage: %s sqlite3_db addr x_offset y_offset z_offset\n", argv[0]);
    exit(1);
  }

  int argc_i = 1;
  std::string filename(argv[argc_i++]);
  Storage storage(filename);

  std::string addr = argv[argc_i++];
  Pixlib::Point point;
  point.x = std::stof(argv[argc_i++]);
  point.y = std::stof(argv[argc_i++]);
  point.z = std::stof(argv[argc_i++]);

  storage.upsert_tracker_service(addr, point);
}



