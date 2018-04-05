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
    // Normal
    glm::vec3 Normal;
    // TexCoords
    glm::vec3 TexCoords;
    // Projection for led rendering
    glm::vec4 framebuffer_proj;
  };

  class LedMesh {
  public:
    LedMesh(const Texture& textures);

    
    // Render the mesh
    void Draw(const IsoCamera& perspective, float brightness);

    size_t numVertices();

    LedVertex getVertex(int idx);
    void addVertex(const LedVertex& vert);
    void addTexture(const Texture& texture);

    void setupMesh();

  protected:
    /*  Render data  */
    GLuint VAO, VBO, EBO;

    Shader shader;

    /*  Mesh Data  */
    std::vector<LedVertex> vertices;
    std::vector<Texture> textures;

  };
}