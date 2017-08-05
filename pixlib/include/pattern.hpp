#pragma once

#include <string>
#include <chrono>

#include <shader.hpp>

class Pattern {
public:
  Pattern(const std::string& fragment_filename);

  std::string getName() const;

  Shader shader;

  float getTimeElapsed() const;

  void resetStart();  
private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start;
  std::string name;
};
