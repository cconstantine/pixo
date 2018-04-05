// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <glm/gtx/vector_angle.hpp>

#include <pixlib/led_mesh.hpp>
#include <pixlib/shader.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

namespace Pixlib {
  LedMesh::LedMesh(const Texture& texture) :
       shader(
  R"(layout (location = 0) in vec3 position;
  layout (location = 1) in vec3 normal;
  layout (location = 2) in vec3 texCoords;
  layout (location = 3) in vec4 framebuf_proj;

  out vec2 TexCoords;

  uniform mat4 view_from;
  uniform mat4 proj_from;

  void main()
  {
      gl_Position = framebuf_proj;

      vec4 texPos = proj_from  * view_from * vec4(position, 1.0f);
      TexCoords =  vec2(texPos.x, -texPos.y) / texPos.z * 0.5 + 0.5 ;
  })",
  R"(in vec2 TexCoords;

  out vec4 color;

  uniform sampler2D texture0;

  uniform float brightness;

  void main()
  {
    if (TexCoords.x >= 0.0f && TexCoords.x <= 1.0f &&
        TexCoords.y >= 0.0f && TexCoords.y <= 1.0f)
      color = texture(texture0, TexCoords) * brightness;
    else
      color = vec4(0.0f);
    })")
  {
    addTexture(texture);
  }
  // Render the mesh
  void LedMesh::Draw(const IsoCamera& perspective, float brightness) {

    shader.Use();

    glUniform1f(glGetUniformLocation(shader.Program, "brightness"), brightness);

    // Transformation matrices
    glm::mat4 led_projection = perspective.GetProjectionMatrix(perspective.getZoom());
    glm::mat4 led_view = perspective.GetViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "proj_from"), 1, GL_FALSE, glm::value_ptr(led_projection));
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view_from"), 1, GL_FALSE, glm::value_ptr(led_view));

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
}
