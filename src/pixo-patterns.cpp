#include <pixlib.hpp>

int main( int argc, char** argv )
{  
  if(argc < 3) {
    fprintf(stderr, "Usage: %s sqlite3_db [patterns]*\n", argv[0]);
    exit(1);
  }

  int argc_i = 1;
  std::string db_filename(argv[argc_i++]);

  std::vector<std::string> filenames;

  for(;argc_i < argc;argc_i++) {
    filenames.push_back(argv[argc_i]);
  }
  Pixlib::PatternCode::upsert(db_filename, filenames);
}



