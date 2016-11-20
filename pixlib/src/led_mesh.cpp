#include <led_mesh.hpp>

// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;
// GL Includes

#include <shader.hpp>

LedMesh::LedMesh(const Texture& texture) {
  ALOGV("LedMesh::LedMesh\n");
  addTexture(texture);
}

// Render the mesh
void LedMesh::Draw(Shader shader) 
{
  // Bind appropriate textures
  for(GLuint i = 0; i < this->textures.size(); i++)
  {
    glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
    // Retrieve texture number (the N in diffuse_textureN)
    stringstream ss;
    ss << "texture" << i; // Transfer GLuint to stream   
    glUniform1i(glGetUniformLocation(shader.Program,  ss.str().c_str()), i);
    glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
  }

  glBindVertexArray(this->VAO);
  glDrawArrays(GL_POINTS, 0, this->vertices.size());
  glBindVertexArray(0);
}

size_t LedMesh::numVertices() 
{
  return this->vertices.size();
}

LedVertex LedMesh::getVertex(int idx) 
{
  return vertices[idx];
}

void LedMesh::addVertex(const LedVertex& vert) {
  vertices.push_back(vert);
}

void LedMesh::addTexture(const Texture& texture) {
  textures.push_back(texture);
}

/*  Functions    */
// Initializes all the buffer objects/arrays
void LedMesh::setupMesh()
{
  // Create buffers/arrays
  glGenVertexArrays(1, &this->VAO);
  glBindVertexArray(this->VAO);

  glGenBuffers(1, &this->VBO);
  glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
  glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(LedVertex), &this->vertices[0], GL_STATIC_DRAW);  

  // Set the vertex attribute pointers
  // Vertex Positions
  glEnableVertexAttribArray(0); 
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LedVertex), (GLvoid*)0);

  // Vertex Normals
  glEnableVertexAttribArray(1); 
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LedVertex), (GLvoid*)offsetof(LedVertex, Normal));

  // Vertex Texture Coords
  glEnableVertexAttribArray(2); 
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(LedVertex), (GLvoid*)offsetof(LedVertex, TexCoords));

  glEnableVertexAttribArray(3); 
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(LedVertex), (GLvoid*)offsetof(LedVertex, framebuffer_proj));

  // Instance projection offset
}
