#pragma once
// Std. Includes
#include <string>
#include <vector>

using namespace std;
// GL Includes
#include <glm/glm.hpp>
#include <assimp/scene.h>

#include <shader.hpp>
#include <drawable.hpp>

struct Vertex {
  // Position
  glm::vec3 Position;
  // Normal
  glm::vec3 Normal;
  // TexCoords
  glm::vec2 TexCoords;
};

struct Texture {
  GLuint id;
  GLenum target;
};

struct Index {
  GLuint idx;
};

class Mesh : public Drawable {
public:
  Mesh(int drawType);

  Mesh(const Texture& textures, int drawType);
  // Constructor
  Mesh(vector<Vertex> vertices, vector<Texture> textures, int drawType);
  Mesh(vector<Vertex> vertices, vector<Texture> textures, vector<Index> indexes, int drawType);

  
  // Render the mesh
  virtual void Draw(Shader shader);

  size_t numVertices();

  Vertex getVertex(int idx);
  void addVertex(const Vertex& vert);
  void addTexture(const Texture& texture);

protected:
  void setupMesh();

  /*  Render data  */
  GLuint VAO, VBO, EBO;

  /*  Mesh Data  */
  vector<Vertex> vertices;
  vector<Texture> textures;
  vector<Index> indices;

  int drawType;

  bool dirtyMesh;
  /*  Functions    */
  // Initializes all the buffer objects/arrays
};
