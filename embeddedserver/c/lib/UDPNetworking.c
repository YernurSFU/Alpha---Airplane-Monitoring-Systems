#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "System.h"
#include "UDPNetworking.h"

#define PORTNUM 12345
#define BUFSIZE 512

static int sockFD;
static socklen_t clientLen;
static struct sockaddr_in clientAddr;
static _Bool isInit = false;
static _Bool isLastSenderSet = false;
static pthread_mutex_t libLock = PTHREAD_MUTEX_INITIALIZER;

void  UDPNetworking_init() {
    pthread_mutex_lock(&libLock);
    if (isInit) {
        pthread_mutex_unlock(&libLock);
        return;
    }

    sockFD = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockFD < 0) {
        pthread_mutex_unlock(&libLock);
        System_exitError("Could not open socket!");
    }
    
    int optVal = 1;
    setsockopt(sockFD, SOL_SOCKET, SO_REUSEADDR, 
         (const void*) &optVal , sizeof(int));

    static struct sockaddr_in serverAddr;
    bzero((char *) &serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons((unsigned short) PORTNUM);

    if (bind(sockFD, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0) {
        pthread_mutex_unlock(&libLock);
        System_exitError("Could not bind socket to server address!");
    }

    clientLen = sizeof(clientAddr);
    isInit = true;
    pthread_mutex_unlock(&libLock);
}

void  UDPNetworking_dtor() {
    pthread_mutex_lock(&libLock);
    if (!isInit) {
        pthread_mutex_unlock(&libLock);
        return;
    }
    isInit = false;
    pthread_mutex_unlock(&libLock);
}

void  UDPNetworking_sendMessage(char* msg) {
    UDPNetworking_init();

    pthread_mutex_lock(&libLock);
    if (!isLastSenderSet) {
        pthread_mutex_unlock(&libLock);
        System_exitError("You should call \"UDPNetworking_getCurrentCommand\" (at least once) before any call to this function!");
    }

    struct sockaddr_in tempClientAddr = clientAddr;
    socklen_t tempClientLen = clientLen;
    pthread_mutex_unlock(&libLock);

    int n = sendto(sockFD, msg, strlen(msg), 0,
        (struct sockaddr*) &tempClientAddr, tempClientLen);

    if (n < 0)
        System_exitError("Could not send message to client!");
}

char* UDPNetworking_getCurrentCommand() {
    UDPNetworking_init();

    char cmdBuf[BUFSIZE];
    bzero(cmdBuf, BUFSIZE);

    struct sockaddr_in tempClientAddr;
    
    pthread_mutex_lock(&libLock);
    socklen_t tempClientLen = clientLen;
    pthread_mutex_unlock(&libLock);

    int n = recvfrom(sockFD, cmdBuf, BUFSIZE, 0,
         (struct sockaddr*) &tempClientAddr, &tempClientLen);
    if (n < 0)
        System_exitError("Could not recieve a new command!");
    
    pthread_mutex_lock(&libLock);
    clientAddr = tempClientAddr;
    clientLen = tempClientLen;
    isLastSenderSet = true;
    pthread_mutex_unlock(&libLock);
    
    return strdup(cmdBuf);
}
