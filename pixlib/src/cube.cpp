#include <pixlib/cube.hpp>

// Std. Includes
#include <vector>
using namespace std;
// GL Includes
#include <opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <pixlib/mesh.hpp>

namespace Pixlib {

  // Constructor, expects a filepath to a 3D model.
  Cube::Cube(const Texture& defaultTexture ) :defaultTexture(defaultTexture),
     shader(
  R"(
  layout (location = 0) in vec3 position;
  layout (location = 1) in vec3 normal;
  layout (location = 2) in vec2 texCoords;
  layout (location = 3) in vec2 texCoordsOffset;
  layout (location = 4) in mat4 positionOffset;

  out vec2 TexCoords;

  uniform mat4 view;
  uniform mat4 projection;

  uniform mat4 MVP;

  void main()
  {
      gl_Position = projection * view  * positionOffset * vec4(position, 1.0f);
      TexCoords = texCoords + texCoordsOffset;
  })",
  R"(
  in vec2 TexCoords;

  out vec4 color;

  uniform sampler2D texture0;

  void main()
  {
    color = texture(texture0, TexCoords);
  })")
  {
    this->load_model();
  }

  int Cube::add_instance(glm::vec3 posDelta, glm::vec2 texDelta, glm::vec3 projDelta) {
    int instance = 0;
    for(GLuint i = 0; i < this->meshes.size(); i++) {
      instance = meshes[i].add_instance(posDelta, texDelta);
    }
    return instance;
  }

  void Cube::move_instance(int instance, const glm::vec3& position) {
    for(GLuint i = 0; i < this->meshes.size(); i++) {
      meshes[i].move_instance(instance, position);
    }
  }

  int Cube::num_instances() {
    return meshes[0].num_instances();
  }

  // Draws the model, and thus all its meshes
  void Cube::draw(const IsoCamera& perspective)
  {
    shader.use();

    // Transformation matrices
    glm::mat4 projection = perspective.get_projection_matrix();
    glm::mat4 view = perspective.get_view_matrix();

    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    for(GLuint i = 0; i < this->meshes.size(); i++) {
      this->meshes[i].draw(shader);
    }
  }

  Texture Cube::get_default_texture() {
    return defaultTexture;
  }

  /*  Functions   */
  // Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
  void Cube::load_model()
  {

    std::vector<Vertex> vertices = std::vector<Vertex>(
      {
       {glm::vec3(+0.005, -0.005, -0.005), glm::vec3(+0.00000000, -1.00000000, +0.00000000), glm::vec2(+0.00000000, +0.00000000)},
       {glm::vec3(+0.005, -0.005, +0.005), glm::vec3(+0.00000000, -1.00000000, +0.00000000), glm::vec2(+0.00000000, +0.00000000)},
       {glm::vec3(-0.005, -0.005, +0.005), glm::vec3(+0.00000000, -1.00000000, +0.00000000), glm::vec2(+0.00000000, +0.00000000)},
       {glm::vec3(-0.005, -0.005, -0.005), glm::vec3(+0.00000000, -1.00000000, +0.00000000), glm::vec2(+0.00000000, +0.00000000)},
       {glm::vec3(+0.005, +0.005, -0.005), glm::vec3(+0.00000000, +1.00000000, +0.00000000), glm::vec2(+0.00000000, +0.00000000)},
       {glm::vec3(-0.005, +0.005, -0.005), glm::vec3(+0.00000000, +1.00000000, +0.00000000), glm::vec2(+0.00000000, +0.00000000)},
       {glm::vec3(-0.005, +0.005, +0.005), glm::vec3(+0.00000000, +1.00000000, +0.00000000), glm::vec2(+0.00000000, +0.00000000)},
       {glm::vec3(+0.005, +0.005, +0.005), glm::vec3(+0.00000000, +1.00000000, +0.00000000), glm::vec2(+0.00000000, +0.00000000)},
       {glm::vec3(+0.005, -0.005, -0.005), glm::vec3(+1.00000000, +0.00000000, +0.00000000), glm::vec2(+0.00000000, +0.00000000)},
       {glm::vec3(+0.005, +0.005, -0.005), glm::vec3(+1.00000000, +0.00000000, +0.00000000), glm::vec2(+0.00000000, +0.00000000)},
       {glm::vec3(+0.005, +0.005, +0.005), glm::vec3(+1.00000000, +0.00000000, +0.00000000), glm::vec2(+0.00000000, +0.00000000)},
       {glm::vec3(+0.005, -0.005, +0.005), glm::vec3(+1.00000000, +0.00000000, +0.00000000), glm::vec2(+0.00000000, +0.00000000)},
       {glm::vec3(+0.005, -0.005, +0.005), glm::vec3(-0.00000000, -0.00000000, +1.00000000), glm::vec2(+0.00000000, +0.00000000)},
       {glm::vec3(+0.005, +0.005, +0.005), glm::vec3(-0.00000000, -0.00000000, +1.00000000), glm::vec2(+0.00000000, +0.00000000)},
       {glm::vec3(-0.005, +0.005, +0.005), glm::vec3(-0.00000000, -0.00000000, +1.00000000), glm::vec2(+0.00000000, +0.00000000)},
       {glm::vec3(-0.005, -0.005, +0.005), glm::vec3(-0.00000000, -0.00000000, +1.00000000), glm::vec2(+0.00000000, +0.00000000)},
       {glm::vec3(-0.005, -0.005, +0.005), glm::vec3(-1.00000000, -0.00000000, -0.00000000), glm::vec2(+0.00000000, +0.00000000)},
       {glm::vec3(-0.005, +0.005, +0.005), glm::vec3(-1.00000000, -0.00000000, -0.00000000), glm::vec2(+0.00000000, +0.00000000)},
       {glm::vec3(-0.005, +0.005, -0.005), glm::vec3(-1.00000000, -0.00000000, -0.00000000), glm::vec2(+0.00000000, +0.00000000)},
       {glm::vec3(-0.005, -0.005, -0.005), glm::vec3(-1.00000000, -0.00000000, -0.00000000), glm::vec2(+0.00000000, +0.00000000)},
       {glm::vec3(+0.005, +0.005, -0.005), glm::vec3(+0.00000000, +0.00000000, -1.00000000), glm::vec2(+0.00000000, +0.00000000)},
       {glm::vec3(+0.005, -0.005, -0.005), glm::vec3(+0.00000000, +0.00000000, -1.00000000), glm::vec2(+0.00000000, +0.00000000)},
       {glm::vec3(-0.005, -0.005, -0.005), glm::vec3(+0.00000000, +0.00000000, -1.00000000), glm::vec2(+0.00000000, +0.00000000)},
       {glm::vec3(-0.005, +0.005, -0.005), glm::vec3(+0.00000000, +0.00000000, -1.00000000), glm::vec2(+0.00000000, +0.00000000)}
      }
     );

    std::vector<GLuint> indices = std::vector<GLuint>(
      { 0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9,10, 8,10,11,12,13,14,12,14,15,16,17,18,16,18,19,20,21,22,20,22,23}
     );

    std::vector<Texture> textures = std::vector<Texture>({get_default_texture()});

    this->meshes.push_back(Mesh(vertices, indices, textures));
    return;
  }
}
