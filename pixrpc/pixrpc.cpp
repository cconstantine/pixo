#include <pixrpc/pixrpc.hpp>

namespace Pixrpc
{

  void serialize (const Location& item, std::ostream& out)
  {
    dlib::serialize(item.x, out);
    dlib::serialize(item.y, out);
    dlib::serialize(item.z, out);
  }

  void deserialize (Location& item, std::istream& in)
  {
    dlib::deserialize(item.x, in);
    dlib::deserialize(item.y, in);
    dlib::deserialize(item.z, in);
  }

  Server::Server(unsigned short port) :
    in(0), out(0)
  {
    bridge.reconfigure(dlib::listen_on_port(port), dlib::transmit(out));
  }

  void Server::send_location(struct Location& loc) {
    out.enqueue_or_timeout(loc, 10);
  }


  Client::Client(const std::string& ip, unsigned short port) :
    in(4), out(4)
  {
    bridge.reconfigure(
      dlib::connect_to_ip_and_port(ip, port),
      dlib::receive(in)
    );
  }

  void Client::receive_location(struct Location& loc) {
    dlib::bridge_status bs = bridge.get_bridge_status();
    in.dequeue(loc);
  }

}