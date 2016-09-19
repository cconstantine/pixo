#pragma once

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
#include <mutex>
#include <condition_variable> 

class Discovery {
public:

  std::string mac_address();

  std::string ip_address();

  void print();

  struct{
    uint8_t mac_address[6];
    uint8_t ip_address[4];
    uint8_t device_type;
    uint8_t protocol_version; // for the device, not the discovery
    uint16_t vendor_id;
    uint16_t product_id;
    uint16_t hw_revision;
    uint16_t sw_revision;
    uint32_t link_speed;

    uint8_t strips_attached;
    uint8_t max_strips_per_packet;
    uint16_t pixels_per_strip;
    uint32_t update_period;
    uint32_t power_total;
    uint32_t delta_sequence;
    uint32_t controller_id;
    uint32_t group_id;
    uint16_t artnet_universe;
    uint16_t artnet_channel;

  } packet;
};


class PixelPusher
{
public:

  PixelPusher();
  ~PixelPusher();

  void update(const Discovery& disc, const asio::ip::udp::endpoint& from);
  void update(int strip, uint8_t* bytes, size_t size, size_t offset);

  void send();
  void send(uint8_t r, uint8_t g, uint8_t b);
  
  void print();

  std::vector<uint8_t> pixels[8];

  int delay;

  int delta_sequence();
private:
  Discovery description;
  asio::ip::udp::endpoint target;
  uint32_t packet_number;

  std::mutex sender_mutex;
  std::condition_variable sender_condition;

  std::thread sender_thread;
  bool working;
  void background_sender();

};

class DiscoveryService
{
public:
  DiscoveryService(short port);
  ~DiscoveryService();

  std::map<std::string, std::shared_ptr<PixelPusher>> pushers;
private:

  void do_receive();
  void thread_method();

  asio::io_service io_service;

  asio::ip::udp::socket socket_;
  asio::ip::udp::endpoint sender_endpoint_;
  std::thread listener;
  bool working;

  Discovery disc;

};