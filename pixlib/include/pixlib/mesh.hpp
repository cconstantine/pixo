#pragma once
// Std. Includes
#include <opengl.h>
#include <string>
#include <vector>

// GL Includes
#include <glm/glm.hpp>

#include <pixlib/texture.hpp>
#include <pixlib/shader.hpp>

namespace Pixlib {
  struct Vertex {
    // Position
    glm::vec3 Position;
    // Normal
    glm::vec3 Normal;
    // TexCoords
    glm::vec2 TexCoords;
  };

  class Mesh {
  public:
    // Constructor
    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);

    // Render the mesh
    void draw(const Shader& shader);

    /*  Render data  */
    GLuint VAO, VBO, EBO, POS, TPOS;
    int  add_instance(const glm::vec3& position, const glm::vec2& textureCoords);
    void move_instance(int instance, const glm::vec3& position);

    int num_instances();
  private:
    void Draw(GLuint program);
    bool dirty;
    /*  Functions    */
    // Initializes all the buffer objects/arrays
    void setup_mesh();

    void update_data();


    /*  Mesh Data  */
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;


    std::vector<glm::mat4> instancePositionOffset;
    std::vector<glm::vec2> instanceTextureOffset;
  };
}