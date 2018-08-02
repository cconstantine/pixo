#include <pixlib/scene.hpp>

namespace Pixlib {
  Scene::Scene() :
    lastTime(std::chrono::steady_clock::now()),
    fps(0),frames(0), render_timer(120), deltaTime(0.0f)
  { }

  void Scene::add_cluster(std::shared_ptr<Drawable> leds)
  {
    screen.models.push_back(leds);
  }

  void Scene::render(const IsoCamera& perspective, float brightness)
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
    screen.render(perspective, brightness);
    render_timer.end();

  }

  float Scene::get_time_delta()
  {
    return deltaTime.count();
  }


  float Scene::get_fps() {
    return fps;
  }

  float Scene::get_render_time()
  {
    return render_timer.duration();
  }
}
