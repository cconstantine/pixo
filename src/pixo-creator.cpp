#include <pixlib.hpp>
#include <storage.hpp>

int main( int argc, char** argv )
{
  if(argc < 3) {
    fprintf(stderr, "Usage: %s sqlite3_db LEDS_PER_SIDE [hostname]*\n", argv[0]);
    exit(1);
  }

  int argc_i = 1;
  std::string filename(argv[argc_i++]);

  const int leds_per_side = atoi(argv[argc_i++]);
  std::vector<std::string> hostnames;

  for(;argc_i < argc;argc_i++) {
    hostnames.push_back(argv[argc_i]);
  }


  Storage storage(filename, Pixlib::Sculpture(1, hostnames, leds_per_side));

  return 0;
}


