#pragma once

#include <string>

#include <opengl.h>

class Shader
{
public:
    GLuint Program;
    // Constructor generates the shader on the fly
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
    Shader(const Shader& copy);

    // Uses the current shader
    void Use() const;

    std::string vertexPath;
    std::string fragmentPath;
};
