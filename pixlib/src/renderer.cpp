#include <renderer.hpp>

#include <glm/gtx/string_cast.hpp>
#include <fstream>


SceneRender::SceneRender() {}

ScreenRender::ScreenRender() :
 shader("../shaders/model_loading.vs", "../shaders/model_loading.frag")
{ }

void ScreenRender::setupLights(IsoCamera& perspective) {
  {
    GLint lightPosLoc        = glGetUniformLocation(shader.Program, "spot_light.position");
    GLint lightSpotdirLoc    = glGetUniformLocation(shader.Program, "spot_light.direction");
    GLint lightSpotCutOffLoc = glGetUniformLocation(shader.Program, "spot_light.cutOff");
    glUniform3f(lightPosLoc,        perspective.Position.x, perspective.Position.y, perspective.Position.z);
    glUniform3f(lightSpotdirLoc,    perspective.Front.x, perspective.Front.y, perspective.Front.z);
    glUniform1f(lightSpotCutOffLoc, glm::cos(glm::radians(perspective.Zoom)));
    glUniform1f(glGetUniformLocation(shader.Program, "spot_light.constant"),  1.0f);
    glUniform1f(glGetUniformLocation(shader.Program, "spot_light.linear"),    0.19);
    glUniform1f(glGetUniformLocation(shader.Program, "spot_light.quadratic"), 0.032);
  }  {
    GLint lightPosLoc        = glGetUniformLocation(shader.Program, "point_light.position");
    GLint lightSpotColor     = glGetUniformLocation(shader.Program, "point_light.color");
    glUniform3f(lightPosLoc,        perspective.Position.x, perspective.Position.y, perspective.Position.z);
    glUniform3f(lightSpotColor,     0.2f, 0.4f, 0.5f);
    glUniform1f(glGetUniformLocation(shader.Program, "point_light.constant"),  1.0f);
    glUniform1f(glGetUniformLocation(shader.Program, "point_light.linear"),    0.009);
    glUniform1f(glGetUniformLocation(shader.Program, "point_light.quadratic"), 0.232);
  }
}

void ScreenRender::render(IsoCamera& perspective, int width, int height) {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glCullFace(GL_BACK);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);


  glViewport(0,0,width,height);

  // Clear the colorbuffer
  glClearColor(0.10f, 0.10f, 0.10f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shader.Use(); 

  setupLights(perspective);

  // Transformation matrices
  glm::mat4 projection = perspective.GetProjectionMatrix(width, height);
  glm::mat4 view = perspective.GetViewMatrix();

  glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
  glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

  for(std::vector<Drawable*>::iterator i = models.begin();i != models.end();i++) {
    Drawable* m = *i;
    m->Draw(shader);
  }

}

FrameBufferRender::FrameBufferRender(const Texture& renderTo) :
  renderedTexture(renderTo),
   width(renderTo.width),
   height(renderTo.height),
   shader("../shaders/leds.vs", "../shaders/leds.frag")
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
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture.id, 0);

  // Set the list of draw buffers.
  GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

  // Always check that our framebuffer is ok
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    fprintf(stderr, "Failed to init GL_FRAMEBUFFER: %d\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    exit(1);
  }

  glGenBuffers(2, pbos);
  glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[0]);
  glBufferData(GL_PIXEL_PACK_BUFFER, 3*width*height, 0, GL_STREAM_READ);
  glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[1]);
  glBufferData(GL_PIXEL_PACK_BUFFER, 3*width*height, 0, GL_STREAM_READ);

  active_pbo = 0;
}

void FrameBufferRender::setupLights(const IsoCamera& perspective) {
  {
    GLint lightPosLoc        = glGetUniformLocation(shader.Program, "spot_light.position");
    GLint lightSpotdirLoc    = glGetUniformLocation(shader.Program, "spot_light.direction");
    GLint lightSpotCutOffLoc = glGetUniformLocation(shader.Program, "spot_light.cutOff");
    glUniform3f(lightPosLoc,        perspective.Position.x, perspective.Position.y, perspective.Position.z);
    glUniform3f(lightSpotdirLoc,    perspective.Front.x, perspective.Front.y, perspective.Front.z);
    glUniform1f(lightSpotCutOffLoc, glm::cos(glm::radians(perspective.Zoom)));
    glUniform1f(glGetUniformLocation(shader.Program, "spot_light.constant"),  1.0f);
    glUniform1f(glGetUniformLocation(shader.Program, "spot_light.linear"),    0.19);
    glUniform1f(glGetUniformLocation(shader.Program, "spot_light.quadratic"), 0.032);
  }  {
    GLint lightPosLoc        = glGetUniformLocation(shader.Program, "point_light.position");
    GLint lightSpotColor     = glGetUniformLocation(shader.Program, "point_light.color");
    glUniform3f(lightPosLoc,        perspective.Position.x, perspective.Position.y, perspective.Position.z);
    glUniform3f(lightSpotColor,     0.2f, 0.4f, 0.5f);
    glUniform1f(glGetUniformLocation(shader.Program, "point_light.constant"),  1.0f);
    glUniform1f(glGetUniformLocation(shader.Program, "point_light.linear"),    0.009);
    glUniform1f(glGetUniformLocation(shader.Program, "point_light.quadratic"), 0.232);
  }
}
void FrameBufferRender::render(const IsoCamera& perspective, uint8_t* buffer, size_t size) {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
  glViewport(0,0,width, height);

  // Clear the colorbuffer
  glClearColor(0.00f, 0.00f, 0.00f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shader.Use();
  
  setupLights(perspective);

  // Transformation matrices
  glm::mat4 projection = camera.GetProjectionMatrix(width, height);
  glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

  glm::mat4 led_projection = glm::perspective(perspective.Zoom, (float)width/(float)height, 0.1f, 1000.0f);// perspective.GetProjectionMatrix(width, height);
  glm::mat4 led_view = perspective.GetViewMatrix();
  glUniformMatrix4fv(glGetUniformLocation(shader.Program, "proj_from"), 1, GL_FALSE, glm::value_ptr(led_projection));
  glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view_from"), 1, GL_FALSE, glm::value_ptr(led_view));

  for(std::vector<Drawable*>::iterator i = models.begin();i != models.end();i++) {
    Drawable* m = *i;
    m->Draw(shader);
  }

  glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[active_pbo]);
  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, 0);
  active_pbo = (active_pbo + 1) % 2;

  glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[active_pbo]);
  GLubyte* src = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
  if(src)
  {
    //send to physical leds
    memcpy(buffer, src, size);
    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
  }
}

Texture FrameBufferRender::getTexture() {
  return renderedTexture;
}


PatternRender::PatternRender(const Texture& renderTo) :
 start(std::chrono::steady_clock::now()),
 width(renderTo.width),
 height(renderTo.height),
 renderedTexture(renderTo)
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
  // Set "renderedTexture" as our colour attachement #0
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture.id, 0);

  // Set the list of draw buffers.
  GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

  // Always check that our framebuffer is ok
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    fprintf(stderr, "Failed to init GL_FRAMEBUFFER\n");
    exit(1);
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
  
  std::chrono::duration<float> diff = std::chrono::steady_clock::now() - start;
  float time_elapsed = diff.count();
  // Use our shader
  pattern.Use();
  glUniform1f(time_id, time_elapsed );
  glUniform2f(resolution_id, width, height);
  glUniform2f(mouse_id, width/2, height/2);

  glUniform1f(itime_id, time_elapsed );
  glUniform2f(iresolution_id, width, height);
  glUniform2f(imouse_id, width/2, height/2);

  // // Clear the screen
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


