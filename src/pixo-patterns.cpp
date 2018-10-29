#include <storage.hpp>

int main( int argc, char** argv )
{  
  if(argc < 3) {
    fprintf(stderr, "Usage: %s sqlite3_db [patterns]*\n", argv[0]);
    exit(1);
  }

  int argc_i = 1;
  std::string filename(argv[argc_i++]);
  Storage storage(filename);

  std::vector<std::string> filenames;

  for(;argc_i < argc;argc_i++) {
    filenames.push_back(argv[argc_i]);
  }
  storage.upsert_patterns(filenames);
}



