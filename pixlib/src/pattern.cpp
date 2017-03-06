#include <pattern.hpp>

Pattern::Pattern(const std::string& fragment_filename) :
 shader("shaders/pattern.frag", fragment_filename.c_str()),
 name(fragment_filename)
{
  for(unsigned int i = shader.fragmentPath.size();i >= 0;i--) {
    if (shader.fragmentPath[i] == '/') {
      break;
    }
    name = shader.fragmentPath[i] + name;
  }
}

std::string Pattern::getName() const {
  return name;
}