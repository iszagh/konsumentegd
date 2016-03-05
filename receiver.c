#include <stdio.h>

#include "egd.h"

int main(int argc, char *argv[])
{
    // printf("Long size: %lu\n", sizeof(unsigned long));
    // printf("Short size: %lu\n", sizeof(unsigned short));
    // printf("Char size: %lu\n", sizeof(unsigned char));
    // printf("Struct size: %lu\n", sizeof(struct GEF_EGD_DATA));

    printf("Listening.\n");
    int retVal = gefSockReceive();
    printf("Receive returned %d.\n", retVal);
    return 0;
}