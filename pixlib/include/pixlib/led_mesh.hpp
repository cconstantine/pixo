#pragma once
// Std. Includes
#include <opengl.h>
#include <string>
#include <vector>

// GL Includes
#include <glm/glm.hpp>

#include <pixlib/shader.hpp>
#include <pixlib/drawable.hpp>
#include <pixlib/mesh.hpp>


namespace Pixlib {
  struct LedVertex {
    // Position
    glm::vec3 Position;
    // TexCoords
    glm::vec3 TexCoords;
    // Projection for led rendering
    glm::vec4 framebuffer_proj;
  };

  class LedMesh {
  public:
    LedMesh();
    
    // Render the mesh
    void draw(const Texture& textures, const IsoCamera& perspective, float brightness, float gamma);

    size_t num_vertices();

    LedVertex get_vertex(int idx);
    void add_vertex(const LedVertex& vert);
    void add_texture(const Texture& texture);

    void setup_mesh();

  protected:
    /*  Render data  */
    GLuint VAO, VBO, EBO;

    Shader shader;

    /*  Mesh Data  */
    std::vector<LedVertex> vertices;

  };
}