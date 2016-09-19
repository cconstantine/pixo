#pragma once
// Std. Includes
#include <string>
#include <map>
#include <vector>
using namespace std;

// GL Includes
#include <assimp/scene.h>

#include <mesh.hpp>
#include <shader.hpp>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Drawable {
public:
    virtual void Draw(Shader shader) = 0;
};
class Model : public Drawable
{
public:
    /*  Functions   */
    // Constructor, expects a filepath to a 3D model.
    Model(const GLchar* path);
    Model(const GLchar* path, const Texture& defaultTexture);
    Model(const GLchar* path, const Texture& defaultTexture, const glm::vec2& defaultTexCoords);

    // Draws the model, and thus all its meshes
    virtual void Draw(Shader shader);
    
    void addInstance(glm::vec3 posDelta, glm::vec2 texDelta, glm::vec3 projDelta);
    int numInstances();


private:
    /*  Model Data  */
    vector<Mesh> meshes;
    string directory;
    vector<Texture> textures_loaded;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    Texture defaultTexture;
    glm::vec2 defaultTexCoords;

    /*  Functions   */
    // Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string path);

    // Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    // Checks all material textures of a given type and loads the textures if they're not loaded yet.
    // The required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};


