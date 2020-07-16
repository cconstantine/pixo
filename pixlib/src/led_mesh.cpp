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
  LedMesh::LedMesh() :
       shader(
  R"(layout (location = 0) in vec3 position;
  layout (location = 1) in vec3 texCoords;
  layout (location = 2) in vec4 framebuf_proj;

  out vec2 TexCoords;
  out float led_brightness;
  out float led_gamma;

  uniform mat4 view_from;
  uniform mat4 proj_from;
  uniform float brightness;
  uniform vec3 camera_pos;
  uniform float gamma;

  void main()
  {
    float led_r = length(camera_pos - position);
    float cam_r = length(camera_pos);
    //float c_i = 1/powe(cam_r);
    //float l_i = 1/pow(led_r, 2);

      gl_Position = framebuf_proj;

      vec4 texPos = proj_from  * view_from * vec4(position, 1.0f);
      TexCoords =  vec2(texPos.x, texPos.y) / texPos.z * 0.5 + 0.5 ;
      led_brightness = brightness * pow(led_r,2) * 1/pow(cam_r, 2) ;
      //led_gamma = gamma;
  })",
  R"(in vec2 TexCoords;
  in float led_brightness;
  in float led_gamma;
  out vec4 color;

  uniform sampler2D texture0;


  void main()
  {
    if (TexCoords.x >= 0.0f && TexCoords.x <= 1.0f &&
        TexCoords.y >= 0.0f && TexCoords.y <= 1.0f)
      color = pow(texture(texture0, TexCoords), vec4(1.0/2.2)) * led_brightness;
    else
      color = vec4(0.0f);
    })")
  { }

  // Render the mesh
  void LedMesh::draw(const Texture& texture, const IsoCamera& perspective, float brightness, float gamma) {

    shader.use();

    glUniform1f(glGetUniformLocation(shader.Program, "brightness"), brightness);
    glUniform1f(glGetUniformLocation(shader.Program, "gamma"), brightness);

    // Transformation matrices
    glm::mat4 led_projection = perspective.get_projection_matrix(perspective.get_zoom());
    glm::mat4 led_view = perspective.get_view_matrix();
    glm::vec3 camera_pos = perspective.Position;

    //fprintf(stderr, "%f, %f, %f\n", perspective.Position.x, perspective.Position.y, perspective.Position.z);

    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "proj_from"), 1, GL_FALSE, glm::value_ptr(led_projection));
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view_from"), 1, GL_FALSE, glm::value_ptr(led_view));
    glUniform3fv(glGetUniformLocation(shader.Program, "camera_pos"), 1, glm::value_ptr(camera_pos));

    // Bind appropriate texture
    glActiveTexture(GL_TEXTURE0); // Active proper texture unit before binding

    glUniform1i(glGetUniformLocation(shader.Program,  "texture0"), 0);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_POINTS, 0, this->vertices.size());
    glBindVertexArray(0);
  }

  size_t LedMesh::num_vertices()
  {
    return this->vertices.size();
  }

  LedVertex LedMesh::get_vertex(int idx)
  {
    return vertices[idx];
  }

  void LedMesh::add_vertex(const LedVertex& vert) {
    vertices.push_back(vert);
  }

  /*  Functions    */
  // Initializes all the buffer objects/arrays
  void LedMesh::setup_mesh()
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

    // Vertex Texture Coords
    glEnableVertexAttribArray(1); 
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LedVertex), (GLvoid*)offsetof(LedVertex, TexCoords));

    glEnableVertexAttribArray(2); 
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(LedVertex), (GLvoid*)offsetof(LedVertex, framebuffer_proj));
  }
}
