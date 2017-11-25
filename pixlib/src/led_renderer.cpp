#include <pixlib/led_renderer.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Pixlib {
  LedRender::LedRender(const Texture& renderTo) :
    renderedTexture(renderTo),
     width(renderTo.width),
     height(renderTo.height),
     shader(
  R"(#version 330 core

  #ifdef GL_ES
  precision highp float;
  #endif

  layout (location = 0) in vec3 position;
  layout (location = 1) in vec3 normal;
  layout (location = 2) in vec3 texCoords;
  layout (location = 3) in vec3 framebuf_proj;

  out vec2 TexCoords;
  out vec3 Position;

  uniform mat4 projection;
  uniform mat4 view_from;
  uniform mat4 proj_from;

  void main()
  {
      gl_Position = projection * vec4(framebuf_proj, 1.0f);

      vec4 texPos = proj_from  * view_from * vec4(position, 1.0f);
      TexCoords =  texPos.xy / texPos.z + 0.5;

      //TexCoords = texCoords;
      Position = position;
  })",
  R"(#version 330 core

  #ifdef GL_ES
  precision highp float;
  #endif

  in vec2 TexCoords;
  in vec3 Position;

  out vec4 color;

  uniform sampler2D texture0;
  uniform sampler2D texture1;

  void main()
  {
    color = texture(texture1, TexCoords);
  })")
  {
    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);


    // Poor filtering
    glBindTexture(GL_TEXTURE_2D, renderedTexture.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT); 

    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture.id, 0);

    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      ALOGV( "Failed to init GL_FRAMEBUFFER: %d\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }


    glGenBuffers(2, pbos);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[0]);
    glBufferData(GL_PIXEL_PACK_BUFFER, 3*width*height, 0, GL_STREAM_READ);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[1]);
    glBufferData(GL_PIXEL_PACK_BUFFER, 3*width*height, 0, GL_STREAM_READ);

    active_pbo = 0;
  }

  void LedRender::render(const IsoCamera& perspective, uint8_t* buffer, size_t size) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    glViewport(0,0,width, height);

    // Clear the colorbuffer
    glClearColor(0.00f, 0.00f, 0.00f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.Use();
    
    // Transformation matrices
    glm::mat4 projection = camera.GetProjectionMatrix(width, height);
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glm::mat4 led_projection = glm::perspective(leds->getZoom(perspective.Position), (float)width/(float)height, 0.1f, 1000.0f);// perspective.GetProjectionMatrix(width, height);
    glm::mat4 led_view = perspective.GetViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "proj_from"), 1, GL_FALSE, glm::value_ptr(led_projection));
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view_from"), 1, GL_FALSE, glm::value_ptr(led_view));

    leds->Draw(shader);

    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[active_pbo]);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, 0);
    active_pbo = (active_pbo + 1) % 2;

    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[active_pbo]);
    GLubyte* src = (GLubyte*)glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0,size, GL_MAP_READ_BIT);
    if(src)
    {
     // ALOGV("Byters: %02x %02x %02x\n", src[0], src[1], src[2]);

      memcpy(buffer, src, size);
      glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    }

  }

  Texture LedRender::getTexture() {
    return renderedTexture;
  }
}
