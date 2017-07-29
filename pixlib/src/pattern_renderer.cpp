#include <pattern_renderer.hpp>

#include <glm/gtx/string_cast.hpp>
#include <fstream>


PatternRender::PatternRender(glm::vec2 canvasSize) :
 start(std::chrono::high_resolution_clock::now()),
 width(canvasSize.x),
 height(canvasSize.y),
 renderedTexture(canvasSize.x, canvasSize.y)
{
  ALOGV("PatternRender::PatternRender (%d x %d)\n", width, height);

  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  
  static const GLfloat g_vertex_buffer_data[] = { 
  // Coordinates
   - 1.0, - 1.0,
     1.0, - 1.0,
   - 1.0,   1.0,

     1.0,   1.0,
     1.0, - 1.0,
   - 1.0,   1.0,
  };
  

  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

  // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
  glGenFramebuffers(1, &FramebufferName);
  glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);


  glBindTexture(GL_TEXTURE_2D, renderedTexture.id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // Set "renderedTexture" as our colour attachement #0
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture.id, 0);

  // Set the list of draw buffers.
  GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

  // Always check that our framebuffer is ok
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    fprintf(stderr, "Failed to init GL_FRAMEBUFFER\n");
  }
}

const Texture& PatternRender::getTexture() {
  return renderedTexture;
}

void PatternRender::render(const Shader& pattern) {
  GLuint time_id = glGetUniformLocation(pattern.Program, "time");
  GLuint resolution_id = glGetUniformLocation(pattern.Program, "resolution");
  GLuint mouse_id = glGetUniformLocation(pattern.Program, "mouse");

  GLuint itime_id = glGetUniformLocation(pattern.Program, "iGlobalTime");
  GLuint iresolution_id = glGetUniformLocation(pattern.Program, "iResolution");
  GLuint imouse_id = glGetUniformLocation(pattern.Program, "iMouse");

  glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
  glViewport(0,0,width, height); // Render on the whole framebuffer, complete from the lower left corner to the upper right

  std::chrono::duration<float> diff = std::chrono::high_resolution_clock::now() - start;
  float time_elapsed = diff.count();

  // Use our shader
  pattern.Use();
  glUniform1f(time_id, time_elapsed );
  glUniform2f(resolution_id, width, height);
  glUniform2f(mouse_id, width/2, height/2);

  glUniform1f(itime_id, time_elapsed );
  glUniform2f(iresolution_id, width, height);
  glUniform2f(imouse_id, width/2, height/2);

   // Clear the screen
  glClear( GL_COLOR_BUFFER_BIT );

  // 1rst attribute buffer : vertices

  glBindVertexArray(VertexArrayID);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // Draw the triangle !
  glDrawArrays(GL_TRIANGLES, 0, 6); // 3 indices starting at 0 -> 1 triangle

  glDisableVertexAttribArray(0);

}

