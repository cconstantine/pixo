#pragma once
// Std. Includes
#include <string>
#include <map>
#include <vector>
using namespace std;

// GL Includes
#include <opengl.h>
#include <mesh.hpp>
#include <shader.hpp>
#include <drawable.hpp>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Cube : public Drawable
{
public:
    /*  Functions   */
    // Constructor, expects a filepath to a 3D model.
    Cube();
    Cube(const Texture& defaultTexture);

    // Draws the model, and thus all its meshes
    virtual void Draw(Shader shader);
    
    int  addInstance(glm::vec3 posDelta, glm::vec2 texDelta, glm::vec3 projDelta);
    void moveInstance(int instance, const glm::vec3& position);

    int numInstances();

    Texture getDefaultTexture();
private:
    /*  Model Data  */
    vector<Mesh> meshes;
    Texture defaultTexture;

    /*  Functions   */
    // Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel();
};


