#include <scene.hpp>

// Include GLFW
#pragma region "User input"

void Scene::mouse_callback(double xoffset, double yoffset)
{
  this->perspective.ProcessMouseMovement(xoffset, yoffset);
} 

void Scene::zoom(double yoffset)
{
  this->perspective.ProcessMouseScroll(yoffset);
}

void Scene::matchViewToPerspective()
{
  viewed_from = perspective;
  flag.moveInstance(0, viewed_from.Position);
}
#pragma endregion


Scene::Scene(ScreenRender* screen, LedCluster* leds) :
  perspective(glm::vec3(-2.168596, 1.416614, 10.411120)), viewed_from(perspective), screen(screen), leds(leds),
  deltaTime(0.0f), lastTime(std::chrono::steady_clock::now()), lastUpdate(std::chrono::steady_clock::now()), next(false),
  flag(), fps(0),frames(0)
{
  flag.addInstance(glm::vec3(), glm::vec2(), glm::vec3() );
  screen->models.push_back(&flag);
  screen->models.push_back(&leds->leds_for_display);

}

void Scene::render(const Shader& pattern, int width, int height)
{
  std::chrono::time_point<std::chrono::steady_clock> currentTime = std::chrono::steady_clock::now();
  ++frames;

  std::chrono::duration<float> time_duration = currentTime - lastUpdate;
  float time_delta = time_duration.count();
  if ( time_delta >= 1.0 ){
    lastUpdate = currentTime;
    float new_fps = frames * (1 / time_delta);
    fps = (fps + 99*new_fps) / 100;
    frames = 0;
  }

  // Set frame time
  deltaTime = currentTime - lastTime;

  lastTime = currentTime;

  leds->render(viewed_from, pattern);
  screen->render(perspective, width, height);

}

float Scene::getFps() {
  return fps;
}

void Scene::Do_Movement(const bool *keys)
{
    // Camera controls
    if(keys[FORWARD]) {
      perspective.ProcessKeyboard(FORWARD, deltaTime.count()*10);
    }
    if(keys[BACKWARD]) {
      perspective.ProcessKeyboard(BACKWARD, deltaTime.count()*10);
    }
    if(keys[LEFT]) {
      perspective.ProcessKeyboard(LEFT, deltaTime.count()*10);
    }
    if(keys[RIGHT]) {
      perspective.ProcessKeyboard(RIGHT, deltaTime.count()*10);
    }

    if(keys[TOWARDS_VIEW]) {
      glm::vec3 delta = viewed_from.Position - perspective.Position;
      

      if(glm::length(delta) > 1.0f) {
        delta = glm::normalize(delta);
      }
      
      delta = glm::vec3(delta.x / 10, delta.y / 10, delta.z / 10);
      perspective.Position = perspective.Position + delta;
    }
}
