#include <webserver.hpp>

#include <crow.h>
#include <thread>

Webserver::Webserver(App *application) :
 manager(&Webserver::run, this),
 application(application)
{

}

Webserver::~Webserver() {
  server.stop();
  manager.join();
}

void Webserver::run() {
  CROW_ROUTE(server, "/")([=](){
    return "Hello world";
  });

  CROW_ROUTE(server, "/status") ([=]{
    crow::json::wvalue x;
    x["pattern"] = application->current_pattern()->getName().c_str();
    x["fps"]     = application->scene_fps();
    x["scene_render_time"] = application->scene_render_time();
    x["led_render_time"] = application->led_render_time();
    return x;
  });

  server.port(18080).multithreaded().run();
}
