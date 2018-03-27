// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <glm/gtx/vector_angle.hpp>

#include <pixlib/led_mesh.hpp>
#include <pixlib/shader.hpp>

using namespace std;

namespace Pixlib {
  LedMesh::LedMesh(const Texture& texture) :
   min_x(0.0f), max_x(0.0f),
   min_y(0.0f), max_y(0.0f),
   min_z(0.0f), max_z(0.0f)
   {
    addTexture(texture);
  }
  // Render the mesh
  void LedMesh::Draw(const Shader& shader) {
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

  float LedMesh::getZoom(glm::vec3 position) 
  {
    float zoom = 0.0f;

    glm::vec3 edge(min_x, min_y, min_z);

    float position_distance = glm::length(position);
    float edge_distance = glm::length(edge);

    glm::vec2 a = glm::normalize(glm::vec2(position_distance, edge_distance ));
    glm::vec2 b = glm::vec2(1, 0.0f);


    zoom = glm::angle(a, b)*2.5;

    return zoom;
  }

  LedVertex LedMesh::getVertex(int idx) 
  {
    return vertices[idx];
  }

  void LedMesh::addVertex(const LedVertex& vert) {
    vertices.push_back(vert);

    if (max_x < vert.Position.x) {
      max_x = vert.Position.x;
    } else if (min_x >  vert.Position.x) {
      min_x = vert.Position.x;
    }
    if (max_y < vert.Position.y) {
      max_y = vert.Position.y;
    } else if (min_y >  vert.Position.y) {
      min_y = vert.Position.y;
    }
    if (max_z < vert.Position.z) {
      max_z = vert.Position.z;
    } else if (min_z >  vert.Position.z) {
      min_z = vert.Position.z;
    }
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
}
