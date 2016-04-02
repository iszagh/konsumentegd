#pragma once

#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h> 
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>

#include "SFML/Graphics.hpp"

#pragma pack(2)  /* change default packing to 2 bytes */
#define GEF_EGD_UDP_DATA_PORT 0x4746

#define EXPECTED_DATA_LENGTH 2


/*
 * https://stackoverflow.com/a/3208376
 */
#define BYTETOBINARYPATTERN "%d%d%d%d%d%d%d%d"
#define BYTETOBINARY(byte)  \
  (byte & 0x80 ? 1 : 0), \
  (byte & 0x40 ? 1 : 0), \
  (byte & 0x20 ? 1 : 0), \
  (byte & 0x10 ? 1 : 0), \
  (byte & 0x08 ? 1 : 0), \
  (byte & 0x04 ? 1 : 0), \
  (byte & 0x02 ? 1 : 0), \
  (byte & 0x01 ? 1 : 0)

struct GEF_EGD_DATA {
    uint16_t pduTypeVersion; /* */
    uint16_t requestId;
    uint32_t producerId;
    uint32_t exchangeId;
    uint32_t timeStampSec;
    uint32_t timeStampNanoSec;
    uint32_t status;
    uint32_t configSignature;
    uint32_t reserved;
    uint8_t productionData[EXPECTED_DATA_LENGTH];
};

#pragma pack()

int gefSockReceive() {
    int socketDesc;
    struct sockaddr_in host_addr;
    struct GEF_EGD_DATA msgEGD;
    //struct in_addr tempAddr;
    int recivedBytes;
    uint8_t printingData[EXPECTED_DATA_LENGTH];

    /* Wypelnianie strukturki adresu */
    memset(&host_addr, 0, sizeof(host_addr));
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(GEF_EGD_UDP_DATA_PORT);
    host_addr.sin_addr.s_addr = INADDR_ANY;






//GUI INIT
    sf::RenderWindow window( sf::VideoMode( 320, 240 ), "Konsument" );



    std::vector <sf::CircleShape> circles (8 * EXPECTED_DATA_LENGTH);

    for (unsigned int i = 0; i < circles.size(); ++i)
    {
      circles.at(i).setPosition((40*i)%(40*8),(i/8)*40);
      circles.at(i).setRadius(20.f);
    }



    while (window.isOpen())
    {
      sf::Event event;
      while (window.pollEvent(event))
      {
        if (event.type == sf::Event::Closed)
        {
          window.close();
        }
      }     


      socketDesc = socket(AF_INET, SOCK_DGRAM, 0);
      if (socketDesc < 0)
      {
        continue;
      }

      if (bind(socketDesc,
      (const struct sockaddr *) &host_addr,
      sizeof(host_addr)))
      {
       // fprintf(stderr, "Error binding socket.\n");
      continue;
      //  return -2;
      }

      memset(&msgEGD, 0, sizeof(msgEGD));

      struct timeval tv;
      tv.tv_sec = 0;
      tv.tv_usec = 100000;

      setsockopt(socketDesc, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval)); //set timeout

      recivedBytes = recv(socketDesc, &msgEGD, sizeof(msgEGD), 0);
      if (recivedBytes == sizeof(msgEGD))
      {

        int i;
        for (i=0; i < EXPECTED_DATA_LENGTH; i++) 
        {
          printf(BYTETOBINARYPATTERN, BYTETOBINARY(msgEGD.productionData[i]));
          printingData[i] = msgEGD.productionData[i];
        }
   
        printf("\n");
      }
      close(socketDesc);


      window.clear();

      char bitCount;
      for (unsigned int i = 0; i < circles.size(); ++i)
      {
        if (i%8 == 0)
          bitCount = 0x1;
        if (printingData[i/8] & bitCount )
          circles.at(i).setFillColor(sf::Color::Green);
        else
          circles.at(i).setFillColor(sf::Color::Red);

        window.draw(circles.at(i));
        bitCount <<= 1;
      }

      window.display();
    }


    return 0;
}
