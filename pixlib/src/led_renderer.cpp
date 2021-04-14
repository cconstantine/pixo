#include <pixlib/led_renderer.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Pixlib {
  LedRender::LedRender(const Texture& renderTo) :
    renderedTexture(renderTo),
     width(renderTo.width),
     height(renderTo.height)
  {
    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture.id, 0);

    // Poor filtering
    glBindTexture(GL_TEXTURE_2D, renderedTexture.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      ALOGV( "Failed to init GL_FRAMEBUFFER for led renderer: %d\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }


    glGenBuffers(2, pbos);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[0]);
    glBufferData(GL_PIXEL_PACK_BUFFER, 3*width*height, 0, GL_STREAM_READ);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[1]);
    glBufferData(GL_PIXEL_PACK_BUFFER, 3*width*height, 0, GL_STREAM_READ);

    active_pbo = 0;
  }

  void LedRender::render(const Texture& texture, const IsoCamera& perspective, float brightness, float gamma, bool overscan, uint8_t* buffer, size_t size) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    glViewport(0,0,width, height);

    // Clear the colorbuffer
    glClearColor(0.00f, 0.00f, 0.00f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    leds->draw(texture, perspective, brightness, gamma, overscan);

    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[active_pbo]);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, 0);
    active_pbo = (active_pbo + 1) % 2;

    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[active_pbo]);
    GLubyte* src = (GLubyte*)glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0,size, GL_MAP_READ_BIT);
    if(src)
    {
      char testblock [size];
      memset(testblock, 0, sizeof testblock);
      memcmp(testblock, buffer, size);
      memcpy(buffer, src, size);
      glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    } else {
      ALOGV("Failed to get frame from GPU\n");
    }
  }
}
