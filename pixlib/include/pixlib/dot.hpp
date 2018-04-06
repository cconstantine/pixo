#pragma once
// Std. Includes
#include <string>
#include <map>
#include <vector>

// GL Includes
#include <opengl.h>
#include <pixlib/mesh.hpp>
#include <pixlib/shader.hpp>
#include <pixlib/drawable.hpp>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Pixlib {

  struct DotVertex {
    // Position
    glm::vec3 Position;
    // TexCoords
    glm::vec2 TexCoords;
  };

  class Dot : public Drawable
  {
  public:
    /*  Functions   */
    Dot(const Texture& defaultTexture);

    // Draws the model, and thus all its meshes
    virtual void draw(const IsoCamera& perspective);
    virtual int  add_instance(glm::vec3 posDelta, glm::vec2 texDelta, glm::vec3 projDelta);

    Texture get_default_texture();
  private:
    bool dirty;
    Texture defaultTexture;

    /*  Render data  */
    GLuint VAO, VBO, EBO;

    Shader shader;

    /*  Mesh Data  */
    std::vector<DotVertex> vertices;
    std::vector<Texture> textures;
  };
}

