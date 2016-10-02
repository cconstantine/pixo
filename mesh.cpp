#include <mesh.hpp>

// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;
// GL Includes
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <shader.hpp>


/*  Functions  */
Mesh::Mesh(const Texture& texture, int drawType) : drawType(drawType), dirtyMesh(true) {
  textures.push_back(texture);
}

// Constructor
Mesh::Mesh(vector<Vertex> vertices, vector<Texture> textures, int drawType) : 
  vertices(vertices), textures(textures), drawType(drawType), dirtyMesh(true)
{
  // fprintf(stderr, "vertices: %d\n", (int)vertices.size());
  // fprintf(stderr, "indices: %d\n", (int)indices.size());
  // fprintf(stderr, "textures: %d\n", (int)textures.size());
  // Now that we have all the required data, set the vertex buffers and its attribute pointers.
}

Mesh::Mesh(vector<Vertex> vertices, vector<Texture> textures, vector<Index> indices, int drawType)  :
 vertices(vertices), textures(textures), indices(indices), drawType(drawType), dirtyMesh(true)
{
  // fprintf(stderr, "vertices: %d\n", (int)vertices.size());
  // fprintf(stderr, "indices: %d\n", (int)indices.size());
  // fprintf(stderr, "textures: %d\n", (int)textures.size());
}

// Render the mesh
void Mesh::Draw(Shader shader) 
{
  if (dirtyMesh) {
    setupMesh();
  }

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

  // Draw mesh
  if (this->indices.size() > 0) {
    glDrawElements(drawType, this->indices.size(), GL_UNSIGNED_INT, 0);
  } else {
    glDrawArrays(drawType, 0, this->vertices.size());
  }
  
  glBindVertexArray(0);

}

size_t Mesh::numVertices() 
{
  return this->vertices.size();
}

Vertex Mesh::getVertex(int idx) 
{
  return vertices[idx];
}

void Mesh::addVertex(const Vertex& vert) {
  dirtyMesh = true;
  vertices.push_back(vert);
}

/*  Functions    */
// Initializes all the buffer objects/arrays
void Mesh::setupMesh()
{
  dirtyMesh = false;
  // Create buffers/arrays
  glGenVertexArrays(1, &this->VAO);
  glBindVertexArray(this->VAO);

  glGenBuffers(1, &this->VBO);
  glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
  glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);  

  if (this->indices.size() > 0) {
    glGenBuffers(1, &this->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(Index), &this->indices[0], GL_STATIC_DRAW);
  }

  // Set the vertex attribute pointers
  // Vertex Positions
  glEnableVertexAttribArray(0); 
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

  // Vertex Normals
  glEnableVertexAttribArray(1); 
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));

  // Vertex Texture Coords
  glEnableVertexAttribArray(2); 
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

  glEnableVertexAttribArray(3); 
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, framebuffer_proj));

  // Instance projection offset
}
