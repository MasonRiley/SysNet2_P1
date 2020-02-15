#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUFFER 500

int main(int argc, char** argv)
{
    struct addrinfo hints;
    struct addrinfo *res, *rp;

    struct sockaddr_storage peer_addr;

    char buffer[BUFFER];
    int port;

    if(argc == 2)
    {
        port = atoi(argv[1]);
    }
    else
    {
        port = 60001;
    }
}
