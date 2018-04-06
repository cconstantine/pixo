#include <pixlib/pattern.hpp>

namespace Pixlib {
  Pattern::Pattern(const std::string& fragment) :
   shader(
  R"(
  layout (location = 0) in vec3 position;
  layout (location = 1) in vec2 surfacePosAttrib;

  out vec2 surfacePosition;

  void main() {
    surfacePosition = position.xy;
    gl_Position = vec4( position, 1.0 );
  })",
  fragment.c_str()),
   start(std::chrono::high_resolution_clock::now())
  {
    ALOGV("Creating pattern\n");
  }

  Pattern::~Pattern() {
    ALOGV("DESTROY pattern\n");
  }

  float Pattern::get_time_elapsed() const {
    std::chrono::duration<float> diff = std::chrono::high_resolution_clock::now() - start;
    return diff.count();
  }
  void Pattern::reset_start() {
    start = std::chrono::high_resolution_clock::now();
  }
}
