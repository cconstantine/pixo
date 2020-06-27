#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>

#include <dlib/bridge.h>
#include <dlib/type_safe_union.h>

namespace Pixrpc {
  struct Location {
    float x, y, z;
  };
  void serialize (const Location& item, std::ostream& out);
  void deserialize (Location& item, std::istream& in);

  class Server {
  public:
    Server(unsigned short port);
    void send_location(struct Location& loc);

  private:
    dlib::pipe<struct Location> in, out;
    dlib::bridge bridge;
  };

  class Client {
  public:
    Client(const std::string& ip, unsigned short port);

    void receive_location(struct Location& loc);
  private:
    dlib::pipe<struct Location> in, out;
    dlib::bridge bridge;
  };
}
