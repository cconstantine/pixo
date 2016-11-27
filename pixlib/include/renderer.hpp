#pragma once
#include <vector>

#include <opengl.h>

#include <shader.hpp>
#include <cube.hpp>
#include <camera.hpp>
#include <chrono>



class SceneRender {
public:
protected:
  SceneRender();
};

class ScreenRender : public SceneRender {

public:
  ScreenRender();

  void render(const IsoCamera& perspective, int width, int height);
  std::vector<Drawable*> models;

private:

  Shader shader;
  void setupLights(const IsoCamera& perspective);
  std::chrono::time_point<std::chrono::steady_clock> lastRender;

};

