#pragma once
// Std. Includes
#include <opengl.h>
#include <string>
#include <vector>

// GL Includes
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
  glm::vec3 TexCoords;
  // Projection for led rendering
  glm::vec3 framebuffer_proj;
};

class LedMesh : public Drawable {
public:
  LedMesh(const Texture& textures);

  
  // Render the mesh
  virtual void Draw(const Shader& shader);

  size_t numVertices();
  float getZoom(glm::vec3 position);

  LedVertex getVertex(int idx);
  void addVertex(const LedVertex& vert);
  void addTexture(const Texture& texture);

  void setupMesh();
protected:
  /*  Render data  */
  GLuint VAO, VBO, EBO;

  /*  Mesh Data  */
  std::vector<LedVertex> vertices;
  std::vector<Texture> textures;

  float min_x;
  float max_x;
  
  float min_y;
  float max_y;
  
  float min_z;
  float max_z;
};
