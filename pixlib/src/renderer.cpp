#include <renderer.hpp>

#include <glm/gtx/string_cast.hpp>
#include <fstream>
#include <thread>


SceneRender::SceneRender() {}

ScreenRender::ScreenRender() :
 shader(
R"(#version 330 core

#ifdef GL_ES
precision highp float;
#endif

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec2 texCoordsOffset;
layout (location = 4) in mat4 positionOffset;

out vec2 TexCoords;
out vec3 Normal;
out vec4 Position;

uniform mat4 view;
uniform mat4 projection;

uniform mat4 MVP;

void main()
{
    gl_Position = projection * view  * positionOffset * vec4(position, 1.0f);
    TexCoords = texCoords + texCoordsOffset;
    Normal = normal;
    Position = positionOffset*vec4(position, 1.0f);
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
{ }

void ScreenRender::setupLights(const IsoCamera& perspective) {
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

void ScreenRender::render(const IsoCamera& perspective, int width, int height) {

  std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
  std::chrono::duration<float> delta = now - lastRender;
  lastRender = now;

  float target = 1.0f/60;
  float remainder = target - delta.count();

  if (remainder > 0)
  {
    std::this_thread::sleep_for(std::chrono::duration<float>(remainder));

  }

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
