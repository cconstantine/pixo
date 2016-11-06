#pragma once
// Std. Includes
#include <string>
#include <vector>

using namespace std;
// GL Includes
#include <opengl.h>
#include <glm/glm.hpp>

#include <shader.hpp>
#include <drawable.hpp>
#include <mesh.hpp>

struct LedVertex {
  // Position
  glm::vec3 Position;
  // Normal
  glm::vec3 Normal;
  // TexCoords
  glm::vec2 TexCoords;
  // Projection for led rendering
  glm::vec3 framebuffer_proj;
};

class LedMesh : public Drawable {
public:
  LedMesh(const Texture& textures);

  
  // Render the mesh
  virtual void Draw(Shader shader);

  size_t numVertices();

  LedVertex getVertex(int idx);
  void addVertex(const LedVertex& vert);
  void addTexture(const Texture& texture);

  void setupMesh();
protected:

  /*  Render data  */
  GLuint VAO, VBO, EBO;

  /*  Mesh Data  */
  vector<LedVertex> vertices;
  vector<Texture> textures;

};
