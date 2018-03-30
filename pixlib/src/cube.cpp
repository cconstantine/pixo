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
  Cube::Cube(const Texture& defaultTexture ) : defaultTexture(defaultTexture)
  {
    this->loadModel();
  }

  int Cube::addInstance(glm::vec3 posDelta, glm::vec2 texDelta, glm::vec3 projDelta) {
    int instance = 0;
    for(GLuint i = 0; i < this->meshes.size(); i++) {
      instance = meshes[i].addInstance(posDelta, texDelta);
    }
    return instance;
  }

  void Cube::moveInstance(int instance, const glm::vec3& position) {
    for(GLuint i = 0; i < this->meshes.size(); i++) {
      meshes[i].moveInstance(instance, position);
    }
  }

  int Cube::numInstances() {
    return meshes[0].numInstances();
  }

  // Draws the model, and thus all its meshes
  void Cube::Draw(const Shader& shader)
  {
    for(GLuint i = 0; i < this->meshes.size(); i++) {
      this->meshes[i].Draw(shader);
    }
  }

  Texture Cube::getDefaultTexture() {
    return defaultTexture;
  }

  /*  Functions   */
  // Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
  void Cube::loadModel()
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

    std::vector<Texture> textures = std::vector<Texture>({getDefaultTexture()});

    this->meshes.push_back(Mesh(vertices, indices, textures));
    return;
  }
}
