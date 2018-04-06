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
  class Cube : public Drawable
  {
  public:
    /*  Functions   */
    Cube(const Texture& defaultTexture);

    // Draws the model, and thus all its meshes
    virtual void draw(const IsoCamera& perspective);
    
    int  add_instance(glm::vec3 posDelta, glm::vec2 texDelta, glm::vec3 projDelta);
    void move_instance(int instance, const glm::vec3& position);

    int num_instances();

    Texture get_default_texture();
  private:
    /*  Model Data  */
    std::vector<Mesh> meshes;
    Texture defaultTexture;

    Shader shader;
    /*  Functions   */
    // Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void load_model();
  };
}

