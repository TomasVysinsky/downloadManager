#include <stdio.h>

#ifndef SERVER
#define SERVER

void mainServer(int port);
void tcpProtokol(int port);
void ftpProtokol(int port);

#endif