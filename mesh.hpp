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
  glm::vec3 framebuffer_proj;
};

struct Texture {
  GLuint id;
  GLenum target;
};

struct Index {
  GLuint idx;
};

class Mesh {
public:
  Mesh(const Texture& textures, int drawType);
  // Constructor
  Mesh(vector<Vertex> vertices, vector<Texture> textures, int drawType);
  Mesh(vector<Vertex> vertices, vector<Texture> textures, vector<Index> indexes, int drawType);

  void setupMesh();
  
  // Render the mesh
  void Draw(Shader shader);

  size_t numVertices();

  Vertex getVertex(int idx);
  void addVertex(const Vertex& vert);

protected:

  /*  Render data  */
  GLuint VAO, VBO, EBO;

  /*  Mesh Data  */
  vector<Vertex> vertices;
  vector<Texture> textures;
  vector<Index> indices;

  int drawType;
  /*  Functions    */
  // Initializes all the buffer objects/arrays
};
