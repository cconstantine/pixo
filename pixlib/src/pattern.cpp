#include <pixlib/pattern.hpp>

#include <fstream>

namespace Pixlib {
  Pattern::Pattern(const std::string& name, const std::string& fragment_code, bool overscan) :
   Shader::Shader(
     R"(
  layout (location = 0) in vec3 position;
  layout (location = 1) in vec2 surfacePosAttrib;

  out vec2 surfacePosition;

  void main() {
    surfacePosition = position.xy;
    gl_Position = vec4( position, 1.0 );
  })",fragment_code.c_str()),
   name(name),
   overscan(overscan),
   width(512),
   height(512),
   renderedTexture(512, 512),
   start(std::chrono::high_resolution_clock::now())

  {
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture.id, 0);

    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      ALOGV(  "Failed to init GL_FRAMEBUFFER for pattern\n");
    }
  }

  const Texture& Pattern::get_texture() const {
    return renderedTexture;
  }

  float Pattern::get_time_elapsed() const {
    std::chrono::duration<float> diff = std::chrono::high_resolution_clock::now() - start;
    return diff.count();
  }
  void Pattern::reset_start() {
    start = std::chrono::high_resolution_clock::now();
  }

  void Pattern::render() {

    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    glViewport(0,0,width, height); // Render on the whole framebuffer, complete from the lower left corner to the upper right

    float time_elapsed = get_time_elapsed();

    // Use our shader
    use();


    GLuint time_id = glGetUniformLocation(Program, "time");
    GLuint resolution_id = glGetUniformLocation(Program, "resolution");
    GLuint mouse_id = glGetUniformLocation(Program, "mouse");

    GLuint itime_id = glGetUniformLocation(Program, "iGlobalTime");
    GLuint iresolution_id = glGetUniformLocation(Program, "iResolution");
    GLuint imouse_id = glGetUniformLocation(Program, "iMouse");


    glUniform1f(time_id, time_elapsed );
    glUniform2f(resolution_id, width, height);
    glUniform2f(mouse_id, width/2, height/2);

    glUniform1f(itime_id, time_elapsed );
    glUniform2f(iresolution_id, width, height);
    glUniform2f(imouse_id, width/2, height/2);

     // Clear the screen
    glClearColor(0.00f, 0.00f, 0.00f, 1.0f);
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
}
