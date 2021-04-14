#pragma once
#include <vector>
#include <chrono>

#include <opengl.h>
#include <glm/glm.hpp>

#include <pixlib/shader.hpp>
#include <pixlib/texture.hpp>


namespace Pixlib {
  class Pattern : public Shader {
  public:
    Pattern(const std::string& name, const std::string& fragment_code, bool overscan);

    void render();

    const Texture& get_texture() const;
    float get_time_elapsed() const;
    void reset_start();

    std::string name;
    bool overscan;

  private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    GLuint VertexArrayID;
    GLuint vertexbuffer;

    GLuint FramebufferName;
    Texture renderedTexture;
    int width, height;


  };
}
