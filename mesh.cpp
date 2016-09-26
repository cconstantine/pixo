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
Mesh::Mesh(int drawType) : drawType(drawType) {}

// Constructor
Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures, int drawType) : drawType(drawType)
{
  this->vertices = vertices;
  this->indices = indices;
  this->textures = textures;

  // fprintf(stderr, "vertices: %d\n", (int)vertices.size());
  // fprintf(stderr, "indices: %d\n", (int)indices.size());
  // fprintf(stderr, "textures: %d\n", (int)textures.size());
  // Now that we have all the required data, set the vertex buffers and its attribute pointers.
}

// Render the mesh
void Mesh::Draw(Shader shader) 
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

  // Draw mesh
  glBindVertexArray(this->VAO);
  glDrawElements(drawType, this->indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

}
/*  Functions    */
// Initializes all the buffer objects/arrays
void Mesh::setupMesh()
{
  // Create buffers/arrays
  glGenVertexArrays(1, &this->VAO);
  glBindVertexArray(this->VAO);

  glGenBuffers(1, &this->VBO);
  glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
  glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);  

  glGenBuffers(1, &this->EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

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
