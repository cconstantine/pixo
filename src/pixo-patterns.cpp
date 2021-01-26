#include <pixpq/pixpq.hpp>
#include <fstream>

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

int main( int argc, char** argv )
{  
  if(argc < 2) {
    fprintf(stderr, "Usage: %s [--disable] [--] [patterns]*\n", argv[0]);
    exit(1);
  }

  bool enabled = std::string(argv[1]) != std::string("--disable");
  int argc_i = 1;

  while(strlen(argv[argc_i]) >= 2 && strncmp(argv[argc_i], "--", 2) == 0) {
    argc_i++;
  }

  printf("Newly inserted patterns will");
  if (!enabled) {
    printf(" NOT");
  }
  printf(" be enabled.\n");

  pixpq::manager manager = pixpq::manager("");
  manager.ensure_schema();
  // pqxx::work w = manager.transaction();

  for(;argc_i < argc;argc_i++) {
    std::string pathname = std::string(argv[argc_i]);
    std::string name = basename(pathname);

    std::ifstream t(pathname);
    std::string code((std::istreambuf_iterator<char>(t)),
                      std::istreambuf_iterator<char>());

      try {
        pixpq::sculpture::pattern p = manager.get<pixpq::sculpture::pattern>(name);  
        p.glsl_code = code;
        printf("Updating  %s\n", name.c_str());
        manager.store<pixpq::sculpture::pattern>(name, p);
      } catch (const pqxx::unexpected_rows &e) {
        pixpq::sculpture::pattern p(code, enabled);\
        printf("Inserting %s\n", name.c_str());
        manager.store<pixpq::sculpture::pattern>(name, p);
      }
  }
}



