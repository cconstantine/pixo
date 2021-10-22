#pragma once
#include <vector>
#include <chrono>

#include <opengl.h>
#include <glm/glm.hpp>

#include <pixlib/shader.hpp>
#include <pixlib/texture.hpp>


namespace Pixlib {
  class Pattern {
  public:
    Pattern(const std::string& name, bool overscan);
    virtual void render() = 0;

    const Texture& get_texture() const;
    float get_time_elapsed() const;
    void reset_start();

    std::string name;
    bool overscan;
  protected:
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    GLuint FramebufferName;
    Texture renderedTexture;
    int width, height;
  };

  class DemoPattern : public Pattern {
  public:
    DemoPattern(const std::string& name, const std::string& fragment_code, bool overscan);
    virtual void render();

  private:
    Shader shader;
    GLuint VertexArrayID;
    GLuint vertexbuffer;
  };
}
