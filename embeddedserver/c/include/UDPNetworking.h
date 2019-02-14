#ifndef UDPNETWORKING_H
#define UDPNETWORKING_H

void  UDPNetworking_init();
void  UDPNetworking_dtor();
void  UDPNetworking_sendMessage(char* msg);
char* UDPNetworking_getCurrentCommand();

#endif
