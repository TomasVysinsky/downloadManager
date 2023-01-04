#include "server.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void mainServer(int port)
{
    int end = 0;
    printf("Welcome to Server\n");
    while (!end)
    {
        printf("Choose type of server you want to launch:\n");
        printf(" 1) TCP\n");
        printf(" 2) FTP\n");
        printf(" 3) HTTP\n");
        int decision = 0;
        scanf("%d", &decision);

        switch (decision) {
            case 1:
                printf("You chose TCP server\n");
                tcpProtokol(port);
                end = 1;
                break;
            case 2:
                printf("FTP server is not avaliable right now. Choose another one.\n");
                break;
            case 3:
                printf("HTTP server is not avaliable right now. Choose another one.\n");
                break;
            default:
                printf("Chose from the avaliable options.\n\n");
        }
    }
}


void tcpProtokol(int port)
{
    printf("Launching TCP server\n");
    int sockfd, newsockfd;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    char buffer[256];

    /*
    if (argc < 2)
    {
        fprintf(stderr,"usage %s port\n", argv[0]);
        return 1;
    }
    */

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    //serv_addr.sin_port = htons(atoi(argv[1]));
    serv_addr.sin_port = htons(port);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
        return;
    }

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error binding socket address");
        return;
    }

    printf("Server listening...\n");
    listen(sockfd, 5);
    cli_len = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_len);
    if (newsockfd < 0)
    {
        perror("ERROR on accept");
        return;
    }
    printf("Connection accepted\n");

    bzero(buffer,256);
    n = read(newsockfd, buffer, 255);
    if (n < 0)
    {
        perror("Error reading from socket");
        return;
    }
    printf("Here is the message: %s\n", buffer);

    const char* msg = "I got your message";
    n = write(newsockfd, msg, strlen(msg)+1);
    if (n < 0)
    {
        perror("Error writing to socket");
        return;
    }

    close(newsockfd);
    close(sockfd);
}