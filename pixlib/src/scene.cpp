#include <scene.hpp>


Scene::Scene(LedCluster* leds) :
  leds(leds),
  lastTime(std::chrono::steady_clock::now()),
  fps(0),frames(0)
{
  screen.models.push_back(leds);

}

void Scene::render(const IsoCamera& perspective, int width, int height)
{
  std::chrono::time_point<std::chrono::steady_clock> currentTime = std::chrono::steady_clock::now();
  ++frames;

  std::chrono::duration<float> time_duration = currentTime - lastUpdate;
  float time_delta = time_duration.count();
  if ( time_delta >= 1.0 ){
    lastUpdate = currentTime;
    fps = frames;
    frames = 0;
  }

  // Set frame time
  deltaTime = currentTime - lastTime;

  lastTime = currentTime;

  screen.render(perspective, width, height);

}

float Scene::getTimeDelta()
{
  return deltaTime.count();
}


float Scene::getFps() {
  return fps;
}
