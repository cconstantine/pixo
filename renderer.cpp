#include <renderer.hpp>

#include <glm/gtx/string_cast.hpp>
#include <fstream>


// Include GLEW
#include <GL/glew.h>

// Include GLFW
#pragma region "User input"

bool keys[1024];


// Is called whenever a key is pressed/released via GLFW
void Scene::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
  if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);

  if(action == GLFW_PRESS) {
      keys[key] = true;  
  }
  else if(action == GLFW_RELEASE) {
      keys[key] = false;
  }
}

void Scene::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
  int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

  static GLfloat lastX = 400, lastY = 300;
  static bool firstMouse = true;

  if (state != GLFW_PRESS) {
    firstMouse = true;
    return;
  }
  if(firstMouse)
  {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  GLfloat xoffset = xpos - lastX;
  GLfloat yoffset = lastY - ypos; 

  lastX = xpos;
  lastY = ypos;

  this->perspective.ProcessMouseMovement(xoffset, yoffset);
} 

void Scene::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
}
void Scene::mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  this->perspective.ProcessMouseScroll(yoffset);
}

#pragma endregion


Scene::Scene(ScreenRender* screen, FrameBufferRender* fb_render) :
  perspective(glm::vec3(0.0f, 1.0f, 2.8f)), viewed_from(glm::vec3(0.0f, 1.0f, 2.8f)), screen(screen), fb_render(fb_render), fps(0),
  deltaTime(0.0f), lastFrame(0.0f), lastTime(glfwGetTime()), lastUpdate(glfwGetTime()), gamma(0.5), next(false),
  flag("../models/cube.obj")
{
  flag.addInstance(glm::vec3(), glm::vec2(), glm::vec3() );
  screen->models.push_back(&flag);
}

void Scene::render() {

 double currentTime = glfwGetTime();
  if ( currentTime - lastUpdate >= 0.1 ){
    lastUpdate = currentTime;
    if (currentTime - lastTime > 0) {
      fps = (fps + (1 /  (currentTime - lastTime))) / 2;
    }
  }
  lastTime = currentTime;

  Do_Movement();
  // Set frame time
  GLfloat currentFrame = glfwGetTime();
  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;

  fb_render->render(viewed_from);
  screen->render(perspective);

}

float Scene::getFps() {
  return fps;
}


float Scene::getGamma()
{
  return gamma;
}

bool Scene::nextPattern() {

  bool ret = keys[GLFW_KEY_ENTER];
  if(ret) {
    keys[GLFW_KEY_ENTER] = false;
  }

  return ret;
}

void Scene::Do_Movement()
{
    // Camera controls
    if(keys[GLFW_KEY_W]) {
      perspective.ProcessKeyboard(FORWARD, deltaTime*10);
    }
    if(keys[GLFW_KEY_S]) {
      perspective.ProcessKeyboard(BACKWARD, deltaTime*10);
    }
    if(keys[GLFW_KEY_A]) {
      perspective.ProcessKeyboard(LEFT, deltaTime*10);
    }
    if(keys[GLFW_KEY_D]) {
      perspective.ProcessKeyboard(RIGHT, deltaTime*10);
    }

    if(keys[GLFW_KEY_LEFT_SHIFT ]) {
      viewed_from = perspective;
      flag.moveInstance(0, viewed_from.Position);
    }

    if(keys[GLFW_KEY_LEFT_CONTROL]) {
      glm::vec3 delta = viewed_from.Position - perspective.Position;
      

      if(glm::length(delta) > 1.0f) {
        delta = glm::normalize(delta);
      }
      
      delta = glm::vec3(delta.x / 10, delta.y / 10, delta.z / 10);
      perspective.Position = perspective.Position + delta;
    }

    for(int i = 48;i <= 57;i++) {
      if (keys[i]) {
        int val = i - 48;
        if(val == 0) {
          val = 10;
        }
        gamma = (float)(val) / 10.0; 
      }
    }
}

SceneRender::SceneRender() {}

ScreenRender::ScreenRender(GLFWwindow* window) :
 window(window),
 shader("../shaders/model_loading.vs", "../shaders/model_loading.frag")
{
  glfwGetFramebufferSize(window, &width, &height);

}

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

void ScreenRender::render(IsoCamera& perspective) {

  glBindFramebuffer(GL_FRAMEBUFFER, 0);


  glfwGetFramebufferSize(window, &width, &height);

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

FrameBufferRender::FrameBufferRender(int width, int height, uint8_t * dest) :
   width(width),
   height(height),
   dest(dest),
   shader("../shaders/leds.vs", "../shaders/leds.frag")
{
  // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
  glGenFramebuffers(1, &FramebufferName);
  glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

  // The texture we're going to render to
  glGenTextures(1, &renderedTexture);
  glBindTexture(GL_TEXTURE_2D, renderedTexture);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

  // Poor filtering
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT); 

  // Set "renderedTexture" as our colour attachement #0
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

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

void FrameBufferRender::setupLights(IsoCamera& perspective) {
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
void FrameBufferRender::render(IsoCamera& perspective) {

  glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
  glViewport(0,0,width, height);

  // Clear the colorbuffer
  glClearColor(0.00f, 0.00f, 0.00f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shader.Use();
  
  setupLights(perspective);

  // Transformation matrices
  glm::mat4 projection = camera.GetProjectionMatrix(width, height);
  glm::mat4 view = camera.GetViewMatrix();
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
    memcpy(dest, src, 3*width*height);
    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
  }
}

Texture FrameBufferRender::getTexture() {
  Texture t;
  t.id = renderedTexture;
  t.target = GL_TEXTURE_2D;
  t.width = width;
  t.height = height;
  return t;
}

PatternRender::PatternRender(int width, int height) : width(width), height(height) {
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

  // The texture we're going to render to
  glGenTextures(1, &renderedTexture);
  glBindTexture(GL_TEXTURE_2D, renderedTexture);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

  // Poor filtering
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // Set "renderedTexture" as our colour attachement #0
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

  // Set the list of draw buffers.
  GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

  // Always check that our framebuffer is ok
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    fprintf(stderr, "Failed to init GL_FRAMEBUFFER\n");
    exit(1);
  }  
}

Texture PatternRender::getTexture() {
  Texture t;
  t.id = renderedTexture;
  t.target = GL_TEXTURE_2D;
  return t;
}

void PatternRender::render(Shader& pattern) {
  GLuint time_id = glGetUniformLocation(pattern.Program, "time");
  GLuint resolution_id = glGetUniformLocation(pattern.Program, "resolution");
  GLuint mouse_id = glGetUniformLocation(pattern.Program, "mouse");

  GLuint itime_id = glGetUniformLocation(pattern.Program, "iGlobalTime");
  GLuint iresolution_id = glGetUniformLocation(pattern.Program, "iResolution");
  GLuint imouse_id = glGetUniformLocation(pattern.Program, "iMouse");

  glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
  glViewport(0,0,width, height); // Render on the whole framebuffer, complete from the lower left corner to the upper right
  
  // Use our shader
  pattern.Use();
  glUniform1f(time_id, glfwGetTime() );
  glUniform2f(resolution_id, width, height);
  glUniform2f(mouse_id, width/2, height/2);

  glUniform1f(itime_id, glfwGetTime() );
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


