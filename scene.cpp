#include <scene.hpp>

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


Scene::Scene(ScreenRender* screen, LedCluster* leds) :
  perspective(glm::vec3(0.0f, 1.0f, 2.8f)), viewed_from(glm::vec3(0.0f, 1.0f, 2.8f)), screen(screen), leds(leds), fps(0),
  deltaTime(0.0f), lastFrame(0.0f), lastTime(glfwGetTime()), lastUpdate(glfwGetTime()), gamma(0.5), next(false),
  flag("../models/cube.obj")
{
  flag.addInstance(glm::vec3(), glm::vec2(), glm::vec3() );
  screen->models.push_back(&flag);
  screen->models.push_back(&leds->leds_for_display);

    // Load models
  //Model display("../models/screen.obj", texture);
  //display.addInstance(glm::vec3(), glm::vec2(1.0, 1.0), glm::vec3());
  //screen_renderer.models.push_back(&display);

  // Model panel("../models/panel.obj", fb_texture);
  // panel.addInstance(glm::vec3(), glm::vec2(0.0, 0.0), glm::vec3());
  // screen_renderer.models.push_back(&panel);


}

void Scene::render(const Shader& pattern) {

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

  leds->render(viewed_from, pattern);
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
