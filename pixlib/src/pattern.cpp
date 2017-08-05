#include <pattern.hpp>

Pattern::Pattern(const std::string& fragment_filename) :
 shader("shaders/pattern.frag", fragment_filename.c_str()),
 name(fragment_filename),
 start(std::chrono::high_resolution_clock::now())
{
  for(unsigned int i = shader.fragmentPath.size();i >= 0;i--) {
    if (shader.fragmentPath[i] == '/') {
      break;
    }
    name = shader.fragmentPath[i] + name;
  }
}

float Pattern::getTimeElapsed() const {
  std::chrono::duration<float> diff = std::chrono::high_resolution_clock::now() - start;
  return diff.count();
}
void Pattern::resetStart() {
  start = std::chrono::high_resolution_clock::now();
}

std::string Pattern::getName() const {
  return name;
}