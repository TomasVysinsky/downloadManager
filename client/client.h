#include <stdio.h>

#ifndef CLIENT
#define CLIENT

void mainClient(char * hostname);
void tcpClient(char * hostname);
void ftpClient(char * hostname);

#endif