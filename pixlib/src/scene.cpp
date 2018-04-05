#include <pixlib/scene.hpp>

namespace Pixlib {
  Scene::Scene() :
    lastTime(std::chrono::steady_clock::now()),
    fps(0),frames(0), render_timer(120)
  { }

  void Scene::addCluster(LedCluster* leds)
  {
    screen.models.push_back(leds->getDrawable());
  }

  void Scene::render(const IsoCamera& perspective)
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

    render_timer.start();
    screen.render(perspective);
    render_timer.end();

  }

  float Scene::getTimeDelta()
  {
    return deltaTime.count();
  }


  float Scene::getFps() {
    return fps;
  }

  float Scene::get_render_time()
  {
    return render_timer.duration();
  }
}
