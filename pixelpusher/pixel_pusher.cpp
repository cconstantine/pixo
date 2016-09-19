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

using asio::ip::udp;

std::string Discovery::mac_address() {
  char buf[64];

  sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x",
    packet.mac_address[0], packet.mac_address[1], packet.mac_address[2],
    packet.mac_address[3], packet.mac_address[4], packet.mac_address[5]);
  return std::string(buf);
}

std::string Discovery::ip_address() {
  char buf[64];

  sprintf(buf, "%d.%d.%d.%d",
    packet.ip_address[0], packet.ip_address[1], packet.ip_address[2], packet.ip_address[3]);
  return std::string(buf);
}

void Discovery::print() {
  fprintf(stderr, " mac_address: %s\n", mac_address().c_str());
  fprintf(stderr, " ip_address: %d.%d.%d.%d\n", 
    packet.ip_address[0], packet.ip_address[1], packet.ip_address[2], packet.ip_address[3]);
  fprintf(stderr, " device_type: %d\n", packet.device_type);
  fprintf(stderr, " protocol_version: %d\n", packet.protocol_version);
  fprintf(stderr, " vendor_id: %d\n", packet.vendor_id);
  fprintf(stderr, " product_id: %d\n", packet.product_id);
  fprintf(stderr, " hw_revision: %d\n", packet.hw_revision);
  fprintf(stderr, " sw_revision: %d\n", packet.sw_revision);
  fprintf(stderr, " link_speed: %d\n", packet.link_speed);
  fprintf(stderr, " strips_attached: %d\n", packet.strips_attached) ;
  fprintf(stderr, " max_strips_per_packet: %d\n", packet.max_strips_per_packet) ;
  fprintf(stderr, " pixels_per_strip: %d\n", packet.pixels_per_strip) ;
  fprintf(stderr, " update_period: %d\n", packet.update_period) ;
  fprintf(stderr, " power_total: %d\n", packet.power_total) ;
  fprintf(stderr, " delta_sequence: %d\n", packet.delta_sequence) ;
  fprintf(stderr, " controller_id: %d\n", packet.controller_id) ;
  fprintf(stderr, " group_id: %d\n", packet.group_id) ;
  fprintf(stderr, " artnet_universe: %d\n", packet.artnet_universe) ;
  fprintf(stderr, " artnet_channel: %d\n", packet.artnet_channel) ;

}



PixelPusher::PixelPusher() :
 packet_number(0), sender_mutex(), sender_condition(), delay(4),
 working(true), sender_thread(&PixelPusher::background_sender, this)
{
  memset(&description, 0, sizeof(description));
}

PixelPusher::~PixelPusher() {
  working = false;
  sender_condition.notify_all();

  sender_thread.join();
}

void PixelPusher::update(const Discovery& disc, const udp::endpoint& from) {
  memcpy(&description.packet, &disc.packet, sizeof(description));
  for(unsigned int i = 0;i < description.packet.strips_attached;i++) {
    pixels[i].resize(3*description.packet.pixels_per_strip);

  }
  target = from;
  target.port(9897);
}

void PixelPusher::update(int strip, uint8_t *bytes, size_t size, size_t offset) {
  size_t to_copy = size;
  if(to_copy > description.packet.pixels_per_strip*3) {
    to_copy = description.packet.pixels_per_strip*3;
  }
  memcpy(&pixels[strip][offset], bytes, to_copy);
}

void PixelPusher::background_sender() {
  std::unique_lock<std::mutex> sender_lock(sender_mutex);
  while(working) {
    sender_condition.wait(sender_lock);
    //fprintf(stderr, "unlocked\n");


    try {
      std::vector<uint8_t> packet;
      asio::io_service io_service;
      udp::socket sock(io_service, udp::endpoint(udp::v4(), 0));

      udp::resolver resolver(io_service);
      udp::endpoint endpoint = *resolver.resolve({udp::v4(),description.ip_address(), "9897"});


      for(unsigned int i = 0; i < description.packet.strips_attached;i+=description.packet.max_strips_per_packet) {
        //dsfprintf(stderr, "i: %d\n", i);
        packet.clear();
        //fprintf(stderr, "packet_number: %d\n", packet_number);
        packet.push_back((packet_number >>  0) & 0xFF);
        packet.push_back((packet_number >>  8) & 0xFF);
        packet.push_back((packet_number >> 16) & 0xFF);
        packet.push_back((packet_number >> 24) & 0xFF);
        packet_number++;

        for(unsigned int j = 0;j < description.packet.max_strips_per_packet;j++) {
          unsigned int idx = i + j;
          packet.push_back((uint8_t)(idx & 0xFF));

          std::copy(pixels[idx].begin(), pixels[idx].end(), std::back_inserter(packet));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(delay));

        sock.send_to(asio::buffer(&packet[0],packet.size()), endpoint);
      }
    } catch (std::exception& e) {
      fprintf(stderr, "Error sending: %s\n", e.what());
    }
  }
}

void PixelPusher::send(uint8_t r, uint8_t g, uint8_t b) {
  for(int strip = 0;strip < description.packet.strips_attached;strip++) {
    for(int i = 0;i < description.packet.pixels_per_strip;i++) {
      pixels[strip][i*3 + 0] = r;
      pixels[strip][i*3 + 1] = g;
      pixels[strip][i*3 + 2] = b;
    }
  }
  send();
}


void PixelPusher::send() {
  //fprintf(stderr, "notifying\n");
  sender_condition.notify_all();
}

void PixelPusher::print() {
  description.print();
  fprintf(stderr, " render delay: %d\n", delay);
  fprintf(stderr, "\n");
}

int PixelPusher::delta_sequence(){
  return description.packet.delta_sequence;
}


DiscoveryService::DiscoveryService(short port)
  : socket_(io_service, udp::endpoint(udp::v4(), port)),
    listener(&DiscoveryService::thread_method, this),working(true)
{  }

DiscoveryService::~DiscoveryService() {
  working = false;

  for (std::map<std::string, std::shared_ptr<PixelPusher>>::iterator it=pushers.begin(); it!=pushers.end(); ++it) {
    std::shared_ptr<PixelPusher> pusher = it->second;
    pusher->send(0, 0, 0);
  }

  io_service.stop();

  if (listener.joinable()) {
    listener.join();
  }
}

std::map<std::string, std::shared_ptr<PixelPusher>> pushers;


void DiscoveryService::do_receive()
{
  socket_.async_receive_from(
    asio::buffer(&disc, sizeof(disc)), sender_endpoint_, [this](std::error_code ec, std::size_t bytes_recvd) {
      if (bytes_recvd > 0) {
        std::string mac_address = disc.mac_address();
        if (pushers.find(mac_address) == pushers.end()) {
          pushers[mac_address] = std::make_shared<PixelPusher>();
        }
        pushers[mac_address]->update(disc, sender_endpoint_);
        
        int delta_sequence = pushers[mac_address]->delta_sequence();
        if(delta_sequence > 3) {
          pushers[mac_address]->delay += 3;
        } else if (pushers[mac_address]->delay > 0 && delta_sequence == 0) {
          pushers[mac_address]->delay -= 1;
        }

        pushers[mac_address]->print();
      }

      if (working) {
        do_receive();
      }
    }
  );
}

void DiscoveryService::thread_method() {
  do_receive();
  io_service.run();
}
