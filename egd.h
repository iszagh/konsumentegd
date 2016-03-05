#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h> 
#include <unistd.h>
#include <time.h>

#pragma pack(2)  /* change default packing to 2 bytes */
#define GEF_EGD_UDP_DATA_PORT 0x4746 
struct GEF_EGD_DATA {
    unsigned short pduTypeVersion; /* */
    unsigned short requestId;
    unsigned long producerId;
    unsigned long exchangeId;
    unsigned long timeStampSec;
    unsigned long timeStampNanoSec;
    unsigned long status;
    unsigned long configSignature;
    unsigned long reserved;
    unsigned char productionData[1400];
};

//#define NOTEDPRINT(x) fprintf(stderr, "%s: x\n", __func__);

int gefSockReceive() {
    int socketDesc;
    struct sockaddr_in host_addr;
    struct GEF_EGD_DATA msgEGD;

    // Wypelnianie strukturki adresu
    memset(&host_addr, 0, sizeof(host_addr));
    host_addr.sin_family = AF_INET;
    host_addr.sin_port   = htons(GEF_EGD_UDP_DATA_PORT);
    host_addr.sin_addr.s_addr = INADDR_ANY;

    // tworzy socket
    // AF_INET - internety a nie domena systemu
    // SOCK_DGRAM - datagramy
    // 0 - protokol defaultowy dla SOCK_DGRAM
    socketDesc = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketDesc < 0) {
        fprintf(stderr, "Could not create socket.\n");
        return -1;
    }

    // Socket od teraz slucha na podanym porcie
    if (bind(socketDesc,
             (const struct sockaddr *) &host_addr,
             sizeof(host_addr)))
    {
        fprintf(stderr, "Error binding socket.\n");
        return -2;
    }

    memset(&msgEGD, 0, sizeof(msgEGD));

    // Blokujacy nasluch na sockecie.
    int recivedBytes = recv(socketDesc, &msgEGD, sizeof(msgEGD), 0);
    if (recivedBytes != sizeof(msgEGD)) {
        fprintf(stderr, "Partial or no read\n");
        close(socketDesc);
        return -3;
    }
    close(socketDesc);

    fprintf(stdout, "%s| Received EGD msg, dumping contents:\n", __func__);
    fprintf(stdout, "pduTypeVersion: %#hx\n", msgEGD.pduTypeVersion);
    fprintf(stdout, "requestId: %#hx\n", msgEGD.requestId);
    struct in_addr tempAddr;
    tempAddr.s_addr = msgEGD.producerId;
    fprintf(stdout, "producerId: %s\n", inet_ntoa(tempAddr));
    fprintf(stdout, "exchangeId: %#lx\n", msgEGD.exchangeId);
    fprintf(stdout, "timeStampSec: %#lx\n", msgEGD.timeStampSec);
    fprintf(stdout, "timeStampNanoSec: %#lx\n", msgEGD.timeStampNanoSec);
    fprintf(stdout, "status: %#lx\n", msgEGD.status);
    fprintf(stdout, "configSignature: %#lx\n", msgEGD.configSignature);
    fprintf(stdout, "reserved: %#lx\n", msgEGD.reserved);
    // unsigned char fprintf(stdout, "" msgEGD.productionData[1400]

    return 0;
}

int gefSockSend() {
    int socketDesc;
    struct sockaddr_in dest_addr;
    struct GEF_EGD_DATA msgEGD;

    // Wypelnianie strukturki adresu
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port   = htons(GEF_EGD_UDP_DATA_PORT);
    inet_aton("127.0.0.1", &dest_addr.sin_addr);

    // Populowanie datagramu EGD
    msgEGD.pduTypeVersion   = 0x010d; /* */
    msgEGD.requestId        = 1;
    msgEGD.producerId       = inet_addr("127.0.0.1");
    msgEGD.exchangeId       = 1;
    msgEGD.timeStampSec     = time(NULL);
    msgEGD.timeStampNanoSec = 0;
    msgEGD.status           = 1;
    msgEGD.configSignature  = 0;
    msgEGD.reserved         = 0;
    // unsigned char msgEGD.productionData = <<to be assigned>>;

    // tworzy socket
    // AF_INET - internety a nie domena systemu
    // SOCK_DGRAM - datagramy
    // 0 - protokol defaultowy dla SOCK_DGRAM
    socketDesc = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketDesc < 0) {
        fprintf(stderr, "Could not create socket.\n");
        return -1;
    }

    int retVal = connect(socketDesc,
                         (const struct sockaddr *) &dest_addr,
                          sizeof(dest_addr));
    if (retVal < 0)
    {
        fprintf(stderr, "%s: Error connecting to socket.\n", __func__);
        close(socketDesc);
        return -2;
    }

    int bytesSend = send(socketDesc, &msgEGD, sizeof(msgEGD), 0);
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