#pragma once

#include <string>

#include <opengl.h>

class Shader
{
public:
    GLuint Program;
    // Constructor generates the shader on the fly
    Shader(const GLchar* vertexCode, const GLchar* fragmentCode);
    ~Shader();

    // Uses the current shader
    void Use() const;
};
