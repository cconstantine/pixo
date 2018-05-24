#pragma once

#include <nanogui/nanogui.h>
#include <pixlib.hpp>
#include <memory>

class WindowApplication : public nanogui::Screen {
public:
  WindowApplication(const std::string& db_filename);
  virtual bool resizeEvent(const nanogui::Vector2i &size);
  
  virtual bool keyboardEvent(int key, int scancode, int action, int modifiers);

  virtual void drawContents();

private:
  std::shared_ptr<Pixlib::App> pixo;
<<<<<<< HEAD
=======

  nanogui::Widget * left_panel;

  nanogui::TextBox *fps_box;
  nanogui::TextBox *scene_time_box;
  nanogui::TextBox *led_time_box;
  nanogui::ImageView *pattern_view;


  nanogui::Widget * main_view;
  PixoWidget* pixo_view;

  nanogui::Label *pattern_label;
>>>>>>> 705cc64... Refactor pattern to be more self-contained.  It is now a shader and able to render itself
};
