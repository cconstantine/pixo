#pragma once
// Std. Includes
#include <opengl.h>
#include <string>
#include <vector>

// GL Includes
#include <glm/glm.hpp>
#include <texture.hpp>

#include <shader.hpp>

struct Vertex {
  // Position
  glm::vec3 Position;
  // Normal
  glm::vec3 Normal;
  // TexCoords
  glm::vec2 TexCoords;
};

class Mesh {
public:
  // Constructor
  Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);

  // Render the mesh
  void Draw(Shader shader);

  /*  Render data  */
  GLuint VAO, VBO, EBO, POS, TPOS;

  /*  Mesh Data  */
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  std::vector<Texture> textures;

  int  addInstance(const glm::vec3& position, const glm::vec2& textureCoords);
  void moveInstance(int instance, const glm::vec3& position);

  int numInstances();
private:
  void Draw(GLuint program);
  bool dirty;
  /*  Functions    */
  // Initializes all the buffer objects/arrays
  void setupMesh();

  void updateData();


  std::vector<glm::mat4> instancePositionOffset;
  std::vector<glm::vec2> instanceTextureOffset;
};
