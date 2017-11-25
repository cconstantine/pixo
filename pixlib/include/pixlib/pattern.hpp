#pragma once

#include <string>
#include <chrono>

#include <pixlib/shader.hpp>

namespace Pixlib {
  class Pattern {
  public:
    Pattern(const std::string& fragment);

    Shader shader;

    float getTimeElapsed() const;

    void resetStart();  
  private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
  };
}
