#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <asio.hpp>
#include <thread>

#include <pixel_pusher.hpp>


int main(void)
{
  DiscoveryService ds(7331);
  //ds.run();
  while(true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000/16));

    std::string mac_address("d8:80:39:65:f1:91");
    if(ds.pushers.find(mac_address) != ds.pushers.end()) {
      std::shared_ptr<PixelPusher> pusher = ds.pushers[mac_address];
      pusher->send();
    }
  }

  return 0;
}