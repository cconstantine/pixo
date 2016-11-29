#include <led_renderer.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

LedRender::LedRender(const Texture& renderTo) :
  renderedTexture(renderTo),
   width(renderTo.width),
   height(renderTo.height),
   shader("shaders/leds.vs", "shaders/leds.frag")
{
  ALOGV("FrameBufferRender::FrameBufferRender\n");

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
    fprintf(stderr, "Failed to init GL_FRAMEBUFFER: %d\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
  }


  glGenBuffers(2, pbos);
  glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[0]);
  glBufferData(GL_PIXEL_PACK_BUFFER, 3*width*height, 0, GL_STREAM_READ);
  glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[1]);
  glBufferData(GL_PIXEL_PACK_BUFFER, 3*width*height, 0, GL_STREAM_READ);

  active_pbo = 0;
}

void LedRender::render(const IsoCamera& perspective, const PatternRender& pattern, uint8_t* buffer, size_t size) {
  // glEnable(GL_DEPTH_TEST);
  // glDepthFunc(GL_LESS);

  // glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
  // glViewport(0,0,width, height);

  // // Clear the colorbuffer
  // glClearColor(0.00f, 0.00f, 0.00f, 1.0f);
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // shader.Use();
  
  // // Transformation matrices
  // glm::mat4 projection = camera.GetProjectionMatrix(width, height);
  // glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

  // glm::mat4 led_projection = glm::perspective(leds->getZoom(perspective.Position), (float)width/(float)height, 0.1f, 1000.0f);// perspective.GetProjectionMatrix(width, height);
  // glm::mat4 led_view = perspective.GetViewMatrix();
  // glUniformMatrix4fv(glGetUniformLocation(shader.Program, "proj_from"), 1, GL_FALSE, glm::value_ptr(led_projection));
  // glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view_from"), 1, GL_FALSE, glm::value_ptr(led_view));

  // Transformation matrices

  glm::mat4 led_projection = glm::perspective(leds->getZoom(perspective.Position), (float)width/(float)height, 0.1f, 1000.0f);// perspective.GetProjectionMatrix(width, height);
  glm::mat4 led_view = perspective.GetViewMatrix();

  for(int i = 0;i < this->leds->vertices.size();++i) {
    LedVertex vert = this->leds->vertices[i];

    glm::vec4 texPos = led_projection  * led_view * glm::vec4(vert.Position, 1.0f);
    glm::vec2 TexCoords = glm::vec2(texPos.x / texPos.z + 0.5f, texPos.y / texPos.z + 0.5f);
    int x = (int)(TexCoords.x + 1.0f) * pattern.width % width;
    int y = (int)(TexCoords.y + 1.0f) * pattern.height % height;
    buffer[i*3 + 0] = pattern.colors[y*pattern.width*3 + x + 0];
    buffer[i*3 + 1] = pattern.colors[y*pattern.width*3 + x + 1];
    buffer[i*3 + 2] = pattern.colors[y*pattern.width*3 + x + 2];
  }
  //ALOGV("buffer[0] color: %2x %2x %2x\n", pattern.colors[0], pattern.colors[1], pattern.colors[2]);
  // leds->Draw(shader);

  // glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[active_pbo]);
  // glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, 0);
  // active_pbo = (active_pbo + 1) % 2;

  // glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[active_pbo]);
  // GLubyte* src = (GLubyte*)glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0,size, GL_MAP_READ_BIT);
  // if(src)
  // {
  //  // ALOGV("Byters: %02x %02x %02x\n", src[0], src[1], src[2]);

  //   memcpy(buffer, src, size);
  //   glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
  // }

}

Texture LedRender::getTexture() {
  return renderedTexture;
}

