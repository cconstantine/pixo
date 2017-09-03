#pragma once

#include <thread>
#include <crow.h>
#include <app.hpp>

class Webserver {
public:
  Webserver(App* application);
  ~Webserver();

private:
  std::thread manager;
  crow::SimpleApp server;
  App *application;

  void run();
};
