#include <pixlib/dot.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// Std. Includes
#include <vector>
using namespace std;
// GL Includes
#include <opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Pixlib {

  // Constructor, expects a filepath to a 3D model.
  Dot::Dot(const Texture& defaultTexture ) :defaultTexture(defaultTexture), dirty(true),
     shader(
  R"(
  layout (location = 0) in vec3 position;
  layout (location = 1) in vec2 texCoords;
  layout (location = 2) in vec2 texCoordsOffset;
  layout (location = 3) in mat4 positionOffset;

  out vec2 TexCoords;

  uniform mat4 view;
  uniform mat4 projection;

  uniform mat4 MVP;

  void main()
  {
    gl_PointSize = 50.0f;
    gl_Position = projection * view  * positionOffset * vec4(position, 1.0f);
    TexCoords = texCoords + texCoordsOffset;
  })",
  R"(
  in vec2 TexCoords;

  out vec4 color;

  uniform sampler2D texture0;

  void main()
  {
    color = texture(texture0, TexCoords);
  })")
  {
    glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(this->VAO);

    glGenBuffers(1, &this->VBO);
    //glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    //glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(DotVertex), &this->vertices[0], GL_STATIC_DRAW);  

    // Set the vertex attribute pointers
    // Vertex Positions
    glEnableVertexAttribArray(0); 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DotVertex), (GLvoid*)0);

    // Vertex Texture Coords
    glEnableVertexAttribArray(1); 
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(DotVertex), (GLvoid*)offsetof(DotVertex, TexCoords));

  }

  int Dot::add_instance(glm::vec3 posDelta, glm::vec2 texDelta, glm::vec3 projDelta) {
    dirty = true;
    vertices.push_back({posDelta, texDelta});

    return 0;
  }

  // Draws the model, and thus all its meshes
  void Dot::draw(const IsoCamera& perspective, float brightness)
  {
    shader.use();
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

    // Transformation matrices
    glm::mat4 projection = perspective.get_projection_matrix();
    glm::mat4 view = perspective.get_view_matrix();

    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

    glBindVertexArray(this->VAO);

    if(dirty) {
      dirty = false;
      ALOGV("verticies: %d\n", vertices.size());

      glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
      glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(DotVertex), &this->vertices[0], GL_STATIC_DRAW);  
    }

    glDrawArrays(GL_POINTS, 0, this->vertices.size());
    glBindVertexArray(0);
  }

  Texture Dot::get_default_texture() {
    return defaultTexture;
  }
}
