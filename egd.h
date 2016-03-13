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

#include "SFML-2.3.2/include/SFML/Graphics.hpp"

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

    /* Wypelnianie strukturki adresu */
    memset(&host_addr, 0, sizeof(host_addr));
    host_addr.sin_family = AF_INET;
    host_addr.sin_port   = htons(GEF_EGD_UDP_DATA_PORT);
    host_addr.sin_addr.s_addr = INADDR_ANY;
    /*
    // tworzy socket
    // AF_INET - internety a nie domena systemu
    // SOCK_DGRAM - datagramy
    // 0 - protokol defaultowy dla SOCK_DGRAM
    */
    
    /*
    // Socket od teraz slucha na podanym porcie
    if (bind(socketDesc,
             (const struct sockaddr *) &host_addr,
             sizeof(host_addr)))
    {
        fprintf(stderr, "Error binding socket.\n");
        return -2;
    }

    memset(&msgEGD, 0, sizeof(msgEGD));

    */
    /*
    // Blokujacy nasluch na sockecie.
    */
 
  //GUI INIT
    sf::RenderWindow window( sf::VideoMode( 320, 240 ), "Konsument" );

    std::vector <sf::CircleShape> circles (8 * EXPECTED_DATA_LENGTH);
    for (unsigned int i = 0; i < circles.size(); ++i)
    {
      circles.at(i).setPosition((40*i)%(40*8),(i/8)*40);
      circles.at(i).setRadius(20.f);
      circles.at(i).setFillColor(sf::Color::Green);
    }

    while (window.isOpen())
    {
      sf::Event event;
      while (window.pollEvent(event))
      {
        if (event.type == sf::Event::Closed)
          window.close();
      }     


      socketDesc = socket(AF_INET, SOCK_DGRAM, 0);
      if (socketDesc < 0) {
   //       fprintf(stderr, "Could not create socket.\n");
  //        return -1;
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
      tv.tv_usec = 1000;

      setsockopt(socketDesc, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval)); //set timeout

      recivedBytes = recv(socketDesc, &msgEGD, sizeof(msgEGD), 0);
      if (recivedBytes == sizeof(msgEGD)) {
       //   fprintf(stderr, "Received bytes: %u\n", recivedBytes);
       //   fprintf(stderr, "Expected bytes: %lu\n", sizeof(msgEGD));
       //   fprintf(stderr, "Partial or no read\n");
       //  close(socketDesc);
       // return -3;

      int i;
      for (i=0; i < EXPECTED_DATA_LENGTH; i++) printf(BYTETOBINARYPATTERN, BYTETOBINARY(msgEGD.productionData[i]));
      printf("\n");
      }
      close(socketDesc);
   
   /*   fprintf(stdout, "%s| Received EGD msg, dumping contents:\n", __func__);
      fprintf(stdout, "pduTypeVersion: %#hx\n", msgEGD.pduTypeVersion);
      fprintf(stdout, "requestId: %#hx\n", msgEGD.requestId);
      tempAddr.s_addr = msgEGD.producerId;
      fprintf(stdout, "producerId: %s\n", inet_ntoa(tempAddr));
      fprintf(stdout, "exchangeId: %u\n", msgEGD.exchangeId);
      fprintf(stdout, "timeStampSec: %#x\n", msgEGD.timeStampSec);
      fprintf(stdout, "timeStampNanoSec: %#x\n", msgEGD.timeStampNanoSec);
      fprintf(stdout, "status: %u\n", msgEGD.status);
      fprintf(stdout, "configSignature: %u\n", msgEGD.configSignature);
      fprintf(stdout, "reserved: %u\n", msgEGD.reserved);
      fprintf(stdout, "data: "); */
   
   

      window.clear();
      for (unsigned int i = 0; i < circles.size(); ++i)
      window.draw(circles.at(i));
      window.display();
    }

    fprintf(stdout, "\nFull EGD structure dump:\n");

    const unsigned char * const px = (unsigned char*)&msgEGD;
    for (unsigned int i=0; i<sizeof(msgEGD); ++i) printf("%02x ", px[i]);

    return 0;
}

int gefSockSend() {
    int socketDesc;
    struct sockaddr_in dest_addr;
    struct GEF_EGD_DATA msgEGD;
    unsigned int bytesSend;
    int retVal;

    /*
    // Wypelnianie strukturki adresu
    */
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port   = htons(GEF_EGD_UDP_DATA_PORT);
    inet_aton("127.0.0.1", &dest_addr.sin_addr);

    /*
    // Populowanie datagramu EGD
    */
    msgEGD.pduTypeVersion   = 0x010d; /* */
    msgEGD.requestId        = 1;
    msgEGD.producerId       = inet_addr("127.0.0.1");
    msgEGD.exchangeId       = 1;
    msgEGD.timeStampSec     = time(NULL);
    msgEGD.timeStampNanoSec = 0;
    msgEGD.status           = 1;
    msgEGD.configSignature  = 0;
    msgEGD.reserved         = 0;
    
    msgEGD.productionData[0] = 0;
    msgEGD.productionData[1] = 1;


    /*
    // tworzy socket
    
    // AF_INET - internety a nie domena systemu
    // SOCK_DGRAM - datagramy
    // 0 - protokol defaultowy dla SOCK_DGRAM
    */
    socketDesc = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketDesc < 0) {
        fprintf(stderr, "Could not create socket.\n");
        return -1;
    }

    retVal = connect(socketDesc,
                     (const struct sockaddr *) &dest_addr,
                     sizeof(dest_addr));

    if (retVal < 0)
    {
        fprintf(stderr, "%s: Error connecting to socket.\n", __func__);
        close(socketDesc);
        return -2;
    }

    bytesSend = send(socketDesc, &msgEGD, sizeof(msgEGD), 0);
    if (bytesSend < 0) {
        fprintf(stderr, "Sending message failed.\n");
        close(socketDesc);
        return -3;
    }
    else if (bytesSend < sizeof(msgEGD)) {
        fprintf(stderr, "Message sent partially.\n");
        close(socketDesc);
        return -4;
    }
    close(socketDesc);

    return 0;
}
