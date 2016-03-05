#include <stdio.h>

#include "egd.h"

int main(int argc, char *argv[])
{
    printf("Sending...\n");
    int retVal = gefSockSend();
    printf("Send returned %d.\n", retVal);
    return 0;
}