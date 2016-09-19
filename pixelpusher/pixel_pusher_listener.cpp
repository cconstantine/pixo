#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <pixel_pusher.h>

typedef struct{
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
} DiscoveryPacket;

#define BUFLEN (sizeof(DiscoveryPacket))
#define PORT 7331

void diep(const char *s)
{
  perror(s);
  exit(1);
}


int main(void)
{
  struct sockaddr_in si_me, si_other;
  int s;
  socklen_t slen=sizeof(si_other);
  unsigned char buf[512];

  if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
    diep("socket");

  memset((void *) &si_me, 0, sizeof(si_me));
  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(PORT);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(s, (struct sockaddr*)&si_me, sizeof(si_me))==-1)
      diep("bind");

  while(1) {
    ssize_t bytes_read = recvfrom(s, &buf, sizeof(buf), 0, (sockaddr*)&si_other, &slen);
    if (bytes_read==-1)
      diep("recvfrom()");
    printf("Received %ld bytes in packet from %s:%d\n", bytes_read,
           inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
    DeviceHeader dh(buf, bytes_read);
    PixelPusher pp(&dh);
    pp.setLogLevel(DEBUG);

    DiscoveryPacket packet;
    memcpy(&packet, buf, sizeof(packet));

    printf("Packet:\n");
    printf(" mac_address: %02x:%02x:%02x:%02x:%02x:%02x\n", 
      packet.mac_address[0], packet.mac_address[1], packet.mac_address[2],
      packet.mac_address[3], packet.mac_address[4], packet.mac_address[5]);
    printf(" ip_address: %d.%d.%d.%d\n", 
      packet.ip_address[0], packet.ip_address[1], packet.ip_address[2], packet.ip_address[3]);
    printf(" device_type: %d\n", packet.device_type);
    printf(" protocol_version: %d\n", packet.protocol_version);
    printf(" vendor_id: %d\n", packet.vendor_id);
    printf(" product_id: %d\n", packet.product_id);
    printf(" hw_revision: %d\n", packet.hw_revision);
    printf(" sw_revision: %d\n", packet.sw_revision);
    printf(" link_speed: %d\n", packet.link_speed);
    printf(" strips_attached: %d\n", packet.strips_attached) ;
    printf(" max_strips_per_packet: %d\n", packet.max_strips_per_packet) ;
    printf(" pixels_per_strip: %d\n", packet.pixels_per_strip) ;
    printf(" update_period: %d\n", packet.update_period) ;
    printf(" power_total: %d\n", packet.power_total) ;
    printf(" delta_sequence: %d\n", packet.delta_sequence) ;
    printf(" controller_id: %d\n", packet.controller_id) ;
    printf(" group_id: %d\n", packet.group_id) ;
    printf(" artnet_universe: %d\n", packet.artnet_universe) ;
    printf(" artnet_channel: %d\n", packet.artnet_channel) ;
    printf(" remaining bytes: %ld\n", bytes_read - sizeof(packet));
    printf("\n");

    // printf("It has %d strips\n",pp.getNumberOfStrips());
    // for(int i = 0; i < pp.getNumberOfStrips();i++) {
    //   pp.getStrip(i)->setPixels(250, 250, 250);
    // }

    // printf("touched: %ld\n", pp.getTouchedStrips().size());
    // pp.sendPacket();

  }


  // struct sockaddr_in s_client;
  // int i, slen=sizeof(s_client);
  // char buf[BUFLEN];

  // memset((char *) &s_client, 0, sizeof(s_client));
  // s_client.sin_family = AF_INET;
  // s_client.sin_port = htons(9897);
  // s_client.sin_addr = si_other.sin_addr;
  
  // printf("Sending packet %d\n", i);
  // sprintf(buf, "This is packet %d\n", i);
  // if (sendto(s, buf, BUFLEN, 0, (sockaddr*)&s_client, slen)==-1)
  //   diep("sendto()");

  close(s);
  return 0;
}