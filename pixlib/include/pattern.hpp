#pragma once

#include <string>

#include <shader.hpp>

class Pattern {
public:
  Pattern(const std::string& fragment_filename);

  std::string getName() const;

  Shader shader;

private:
  std::string name;
};
