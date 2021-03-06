#include <pixlib/renderer.hpp>

#include <glm/gtx/string_cast.hpp>
#include <fstream>
#include <thread>

namespace Pixlib {

  ScreenRender::ScreenRender() : lastRender(std::chrono::high_resolution_clock::now())
  { }

  void ScreenRender::render(const IsoCamera& perspective, float brightness) {

    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> delta = now - lastRender;

    float target = 1.0f/60;
    float remainder = target - delta.count();
    if (remainder > 0) {
      std::this_thread::sleep_for(std::chrono::duration<float>(remainder ));
    }
    lastRender = std::chrono::high_resolution_clock::now();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0,perspective.width,perspective.height);

    // Clear the colorbuffer
    glClearColor(0.10f, 0.10f, 0.10f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(std::shared_ptr<Drawable> m : models) {
      m->draw(perspective, brightness);
    }
  }
}
