#pragma once

#include <string>
#include <chrono>

#include <pixlib/shader.hpp>

namespace Pixlib {
  class Pattern {
  public:
    Pattern(const std::string& fragment);
    ~Pattern();

    float get_time_elapsed() const;
    void reset_start();

    Shader shader;
  private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
  };
}
