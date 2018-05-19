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
};
