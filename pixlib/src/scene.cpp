#include <scene.hpp>


Scene::Scene(ScreenRender* screen, LedCluster* leds) :
  screen(screen), leds(leds),
  deltaTime(0.0f), lastTime(std::chrono::steady_clock::now()), lastUpdate(std::chrono::steady_clock::now()), next(false),
  fps(0),frames(0)
{
  screen->models.push_back(&leds->leds_for_display);

}

void Scene::render(const IsoCamera& perspective, int width, int height)
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

  screen->render(perspective, width, height);

}

float Scene::getFps() {
  return fps;
}
