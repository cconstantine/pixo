#pragma once
// Std. Includes
#include <string>
#include <vector>

using namespace std;
// GL Includes
#include <glm/glm.hpp>
#include <assimp/scene.h>

#include <shader.hpp>
struct Vertex {
  // Position
  glm::vec3 Position;
  // Normal
  glm::vec3 Normal;
  // TexCoords
  glm::vec2 TexCoords;
  // Projection for led rendering
  glm::vec3 proj;
};

struct Texture {
  GLuint id;
  GLenum target;
};

class Mesh {
public:
  Mesh(int drawType);
  // Constructor
  Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures, int drawType);

  void setupMesh();
  
  // Render the mesh
  void Draw(Shader shader);

  /*  Render data  */
  GLuint VAO, VBO, EBO;

  /*  Mesh Data  */
  vector<Vertex> vertices;
  vector<GLuint> indices;
  vector<Texture> textures;

  vector<glm::vec3> instanceProj;

  int drawType;

protected:

  /*  Functions    */
  // Initializes all the buffer objects/arrays
};
