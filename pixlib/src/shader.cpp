#include <shader.hpp>

Shader::Shader(const Shader& copy) : Program(copy.Program) { }

Shader::Shader(const GLchar* vShaderCode, const GLchar* fShaderCode) {
 
  GLuint vertex, fragment;
  GLint success;
  GLchar infoLog[512];
  // Vertex Shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, NULL);
  glCompileShader(vertex);
  // Print compile errors if any
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    ALOGV("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
  }
  // Fragment Shader
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, NULL);
  glCompileShader(fragment);
  // Print compile errors if any
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    ALOGV("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
  }
  // Shader Program
  this->Program = glCreateProgram();
  glAttachShader(this->Program, vertex);
  glAttachShader(this->Program, fragment);
  glLinkProgram(this->Program);
  // Print linking errors if any
  glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
  if (!success)
  {
    glGetProgramInfoLog(this->Program, 512, NULL, infoLog);

    ALOGV("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
  }
  // Delete the shaders as they're linked into our program now and no longer necessery
  glDeleteShader(vertex);
  glDeleteShader(fragment);

}

// Uses the current shader
void Shader::Use() const { 
    glUseProgram(this->Program); 
}
