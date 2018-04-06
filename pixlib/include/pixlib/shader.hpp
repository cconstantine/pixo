#pragma once

#include <string>

#include <opengl.h>

namespace Pixlib {
  class Shader
  {
  public:
    static std::string ShaderPreamble;
      GLuint Program;
      // Constructor generates the shader on the fly
      Shader(const GLchar* vertexCode, const GLchar* fragmentCode);
      ~Shader();

      // Uses the current shader
      void use() const;
  };
}
