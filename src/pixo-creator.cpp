#include <pixlib.hpp>
#include <storage.hpp>

std::string Pixlib::Shader::ShaderPreamble = "#version 330\n";

int main( int argc, char** argv )
{
  bool reverse = false;
  if(argc < 3) {
    fprintf(stderr, "Usage: %s sqlite3_db LEDS_PER_SIDE [--reverse] [hostname]*\n", argv[0]);
    exit(1);
  }

  int argc_i = 1;
  std::string filename(argv[argc_i++]);

  const int leds_per_side = atoi(argv[argc_i++]);
  std::vector<std::string> hostnames;

  if (std::string("--reverse") == std::string(argv[argc_i])) {
    reverse = true;
    argc_i++;
  }

  for(;argc_i < argc;argc_i++) {
    hostnames.push_back(argv[argc_i]);
  }

  Storage storage(filename, Pixlib::Sculpture(1, hostnames, leds_per_side, reverse));

  return 0;
}


