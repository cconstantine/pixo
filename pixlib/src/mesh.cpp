// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
// GL Includes
#include <opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <pixlib/shader.hpp>
#include <pixlib/mesh.hpp>

using namespace std;

namespace Pixlib {
  /*  Functions  */
  // Constructor
  Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures) : dirty(true)
  {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
  }

  // Render the mesh
  void Mesh::Draw(const Shader& shader) {
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
    if (dirty) {
      updateData();
    }
    // Draw mesh
    glBindVertexArray(this->VAO);
    glDrawElementsInstanced(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0, instanceTextureOffset.size());
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
    glGenBuffers(1, &this->EBO);
    glGenBuffers(1, &this->POS);
    glGenBuffers(1, &this->TPOS);

    // Load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

    // Set the vertex attribute pointers
    // Vertex Positions
    glEnableVertexAttribArray(0); 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glVertexAttribDivisor(0, 0);
    // Vertex Normals
    glEnableVertexAttribArray(1); 
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
    glVertexAttribDivisor(1, 0);
    // Vertex Texture Coords
    glEnableVertexAttribArray(2); 
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));
    glVertexAttribDivisor(2, 0);

    // Instance Texture Offsets
    glBindBuffer(GL_ARRAY_BUFFER, this->TPOS);
    glEnableVertexAttribArray(3); 
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (GLvoid*)0);
    glVertexAttribDivisor(3, 1);

    GLsizei vec4Size = sizeof(glm::vec4);

    
    
    // Instance position offsets
    glBindBuffer(GL_ARRAY_BUFFER, this->POS);
    glEnableVertexAttribArray(4); 
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)0);
    glVertexAttribDivisor(4, 1);

    glEnableVertexAttribArray(5); 
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(1l * vec4Size));
    glVertexAttribDivisor(5, 1);

    glEnableVertexAttribArray(6); 
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(2l * vec4Size));
    glVertexAttribDivisor(6, 1);

    glEnableVertexAttribArray(7); 
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(3l * vec4Size));
    glVertexAttribDivisor(7, 1);

  }

  void Mesh::updateData()
  {
    dirty = false;
    glBindVertexArray(this->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->POS);
    glBufferData(GL_ARRAY_BUFFER, instancePositionOffset.size() * sizeof(glm::mat4), &instancePositionOffset[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, this->TPOS);
    glBufferData(GL_ARRAY_BUFFER, instanceTextureOffset.size() * sizeof(glm::vec2), &instanceTextureOffset[0], GL_STATIC_DRAW);
  }

  void Mesh::moveInstance(int i, const glm::vec3& position) {
    instancePositionOffset[i] = glm::translate(glm::mat4(), position );
    dirty = true;
  }

  int Mesh::addInstance(const glm::vec3& position, const glm::vec2& textureCoords)
  {
    dirty = true;
    instancePositionOffset.push_back(glm::translate(glm::mat4(), position ));
    instanceTextureOffset.push_back( textureCoords);
    return 0;//numInstances() - 1;
  }

  int Mesh::numInstances()
  {
    instancePositionOffset.size();
  }
}