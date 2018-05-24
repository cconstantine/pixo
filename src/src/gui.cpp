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
  
<<<<<<< HEAD
  this->setLayout(new BoxLayout(Orientation::Horizontal));
  /* No need to store a pointer, the data structure will be automatically
     freed when the parent window is deleted */
  new Label(this, "Push buttons", "sans-bold");

  Button *b = new Button(this, "Plain button");
  b->setTooltip("short tooltip");
  new PixoWidget(this, pixo);
=======
  this->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Minimum));

  left_panel = new Widget(this);
  left_panel->setLayout(new BoxLayout(Orientation::Vertical, Alignment::Middle));

  {
    Widget* rates = new Widget(left_panel);
    GridLayout *layout = new GridLayout(
                                    Orientation::Horizontal, 2,
                                    Alignment::Middle, 15, 5);
    layout->setColAlignment({ Alignment::Maximum, Alignment::Fill });
    layout->setSpacing(0, 10);

    rates->setLayout(layout);
    new Label(rates, "Frame rate :", "sans-bold");
              fps_box = new TextBox(rates);
              fps_box->setEditable(false);
              fps_box->setFixedSize(Vector2i(100, 20));
              fps_box->setUnits("fps");
              fps_box->setFontSize(16);
    new Label(rates, "Screen render :", "sans-bold");
              scene_time_box = new TextBox(rates);
              scene_time_box->setEditable(false);
              scene_time_box->setFixedSize(Vector2i(100, 20));
              scene_time_box->setUnits("ms");
              scene_time_box->setFontSize(16);
    new Label(rates, "LED render :", "sans-bold");
              led_time_box = new TextBox(rates);
              led_time_box->setEditable(false);
              led_time_box->setFixedSize(Vector2i(100, 20));
              led_time_box->setUnits("ms");
              led_time_box->setFontSize(16);
  }

  {
    pattern_view = new ImageView(left_panel, pixo->get_pattern_texture().id);
    pattern_view->setFixedSize(Eigen::Vector2i(190, 190));
  }
  main_view = new Widget(this);
>>>>>>> 705cc64... Refactor pattern to be more self-contained.  It is now a shader and able to render itself


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

<<<<<<< HEAD
=======
void WindowApplication::update_labels() {
  char buf[256];
  sprintf(buf, "%2.02f", pixo->scene_fps());
  fps_box->setValue(buf);

  sprintf(buf, "%2.02f", pixo->scene_render_time()*1000);
  scene_time_box->setValue(buf);

  sprintf(buf, "%2.02f", pixo->led_render_time()*1000);
  led_time_box->setValue(buf);

  pattern_label->setCaption(pixo->get_pattern().name);
  
  pattern_view->fit();
  pattern_view->bindImage(pixo->get_pattern_texture().id);

}

>>>>>>> 705cc64... Refactor pattern to be more self-contained.  It is now a shader and able to render itself
void WindowApplication::drawContents() {
  using namespace nanogui; 


  pixo->render_leds();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  ALOGV("led_render_time: %fms\n", pixo->led_render_time()*1000);
  ALOGV("led_render_time: %fms\n", pixo->scene_render_time()*1000);

}
