#include <pixlib/app.hpp>

namespace Pixlib {
  App::App(glm::vec2 canvas_size) :
   scene(),
   pattern_render(canvas_size)
  {  }

  App::~App() {
    for (LedCluster* i : led_clusters) {
      delete i;
    }
  }


  void App::BuildPixo(const std::vector<FadeCandy*>& fadecandies, unsigned int per_size) {
    float spacing = 0.04318;

    int width = per_size;
    int height = per_size;
    int depth = per_size;

    float x_offset = -(float)width  / 2.0f  + 0.5f;
    float y_offset = -(float)height / 2.0f - 0.5f;
    float z_offset = -(float)depth  / 2.0f  + 0.5f;

    viewed_from.scope = glm::vec3(x_offset, y_offset, z_offset)*-spacing;
    camera.scope      = glm::vec3(x_offset, y_offset, z_offset)*-spacing;

    int num_fadecandies = fadecandies.size();
    int per_fc = height / num_fadecandies;

    for(int y = height;y > 0;y--) {
      int direction = 1;
      int selection = (y-1) / per_fc;

      FadeCandy* fc = fadecandies[selection];

      for(int z = 0;z < height;z++) {
        for(int x = std::max(-direction * (width - 1), 0); x >= 0 && x < width;x+=direction) {
          fc->addLed(glm::vec3( ((float)x + x_offset)*spacing,
                                ((float)z + z_offset)*spacing,
                                ((float)y + y_offset)*spacing));
        }
        direction *= -1;
      }
    }

    for(int i = 0;i < num_fadecandies;i++) {
      fadecandies[i]->finalize();

      addFadeCandy(fadecandies[i]);
    }

  }

  void App::addFadeCandy(FadeCandy* fc) {
    LedCluster *lc = new LedCluster(fc, pattern_render.getTexture());

    scene.addCluster(lc);
    led_clusters.push_back(lc);
  }

  float App::scene_fps() {
    return scene.getFps();
  }

  float App::scene_render_time() {
    return scene.get_render_time();
  }

  float App::led_render_time() {
    float total = 0;
    for (LedCluster* i : led_clusters) {
      total += i->render_time();
    }
    return total;
  }

  void App::ProcessMouseMovement(int xoffset, int yoffset) {
    camera.ProcessMouseMovement(xoffset, yoffset);
  }


  void App::ProcessMouseScroll(float x) {
    camera.ProcessMouseScroll(x);
  }

  void App::move_perspective_to_camera() {
    viewed_from.moveTowards(camera, scene.getTimeDelta()*0.8);
  }

  const Texture& App::getPatternTexture() {
    return pattern_render.getTexture();
  }

  void App::setScreenSize(int width, int height) {
    camera.width = width;
    camera.height = height;
  }

  void App::tick(Pattern* pattern, float brightness) {
    pattern_render.render(*pattern);

    for (LedCluster* led_cluster : led_clusters) {
      led_cluster->render(viewed_from, *pattern, brightness);
    }
    scene.render(camera);
  }
}
