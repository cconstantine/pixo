#pragma once
#include <vector>

#include <opengl.h>

#include <shader.hpp>
#include <cube.hpp>
#include <camera.hpp>
#include <chrono>



class SceneRender {
public:
  std::vector<Drawable*> models;
protected:
  SceneRender();
};

class ScreenRender : public SceneRender {

public:
  ScreenRender();

  void render(const IsoCamera& perspective, int width, int height);

  Shader shader;
  void setupLights(const IsoCamera& perspective);
};

