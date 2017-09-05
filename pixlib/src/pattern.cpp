#include <pixlib/pattern.hpp>

namespace Pixlib {
  Pattern::Pattern(const std::string& name, const std::string& fragment) :
   shader(
  R"(#version 330 core

  #ifdef GL_ES
  precision highp float;
  #endif

  layout (location = 0) in vec3 position;
  layout (location = 1) in vec2 surfacePosAttrib;

  out vec2 surfacePosition;

  void main() {
    surfacePosition = position.xy;
    gl_Position = vec4( position, 1.0 );
  })",
  fragment.c_str()),
   name(name),
   start(std::chrono::high_resolution_clock::now())
  { }

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
}
