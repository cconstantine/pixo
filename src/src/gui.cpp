#include <gui.hpp>

#pragma once

#include <nanogui/nanogui.h>
#include <pixlib.hpp>


class PixoWidget : public nanogui::GLCanvas {
public:
    PixoWidget(Widget *parent, std::shared_ptr<Pixlib::App> pixo)
     : nanogui::GLCanvas(parent),
       pixo(pixo)
    { }

    virtual void drawGL() override {
      nanogui::Vector2i pos = absolutePosition();
      nanogui::Vector2i s = size();

      //glViewport(pos.x(),pos.y(), s.x(), s.y());
      pixo->set_screen_size(s.x(), s.y());

      pixo->render_scene();
    }

private:
  std::shared_ptr<Pixlib::App> pixo;
};


WindowApplication::WindowApplication(const std::string& db_filename) 
  : nanogui::Screen(Eigen::Vector2i(200, 200), "Pixo"),
    pixo(std::make_shared<Pixlib::App>(Pixlib::Storage(db_filename)))
{
  using namespace nanogui;
  
  this->setLayout(new BoxLayout(Orientation::Horizontal));
  /* No need to store a pointer, the data structure will be automatically
     freed when the parent window is deleted */
  new Label(this, "Push buttons", "sans-bold");

  Button *b = new Button(this, "Plain button");
  b->setTooltip("short tooltip");
  new PixoWidget(this, pixo);


  performLayout();

}

bool WindowApplication::resizeEvent(const nanogui::Vector2i &size)
{
  nanogui::Screen::resizeEvent(size);
  performLayout();
}

bool WindowApplication::keyboardEvent(int key, int scancode, int action, int modifiers)
{
  if (Screen::keyboardEvent(key, scancode, action, modifiers))
    return true;
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    setVisible(false);
    return true;
  }
  return false;
}

void WindowApplication::drawContents() {
  using namespace nanogui; 


  pixo->render_leds();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  ALOGV("led_render_time: %fms\n", pixo->led_render_time()*1000);
  ALOGV("led_render_time: %fms\n", pixo->scene_render_time()*1000);

}
