#include <stdio.h>

#include "egd.h"

int gefSockSend()
{
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
    
    msgEGD.productionData[0] = 1;
    msgEGD.productionData[1] = 255;


    /*
    // tworzy socket
    
    // AF_INET - internety a nie domena systemu
    // SOCK_DGRAM - datagramy
    // 0 - protokol defaultowy dla SOCK_DGRAM
    */
    socketDesc = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketDesc < 0)
    {
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
    if (bytesSend < 0)
    {
        fprintf(stderr, "Sending message failed.\n");
        close(socketDesc);
        return -3;
    }
    else if (bytesSend < sizeof(msgEGD))
    {
        fprintf(stderr, "Message sent partially.\n");
        close(socketDesc);
        return -4;
    }
    close(socketDesc);

    return 0;
}


int main(int argc, char *argv[])
{
    printf("Sending...\n");
    int retVal = gefSockSend();
    printf("Send returned %d.\n", retVal);
    return 0;
}
