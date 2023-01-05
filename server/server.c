#include "server.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*FTP server*/
/*for getting file size using stat()*/
#include<sys/stat.h>

/*for sendfile()*/
#include<sys/sendfile.h>

/*for O_RDONLY*/
#include<fcntl.h>

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
                //printf("FTP server is not avaliable right now. Choose another one.\n");
                printf("You chose FTP server\n");
                ftpProtokol(21);
                end = 1;
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

void ftpProtokol(int port)
{
    //TODO skontrolovat ako funguje tato metoda
    struct sockaddr_in server, client;
    struct stat obj;
    int sock1, sock2;
    char buf[100], command[5], filename[20];
    int k, i, size, len, c;
    int filehandle;
    sock1 = socket(AF_INET, SOCK_STREAM, 0);
    if(sock1 == -1)
    {
        printf("Socket creation failed\n");
        exit(1);
    }
    bzero((char*)&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(1026);
    server.sin_addr.s_addr = INADDR_ANY;
    k = bind(sock1,(struct sockaddr*)&server,sizeof(server));
    if(k == -1)
    {
        printf("Binding error\n");
        exit(1);
    }
    printf("Server listening...\n");
    k = listen(sock1,1);
    if(k == -1)
    {
        printf("Listen failed\n");
        exit(1);
    }
    len = sizeof(client);
    printf("Server accepting...\n");
    sock2 = accept(sock1,(struct sockaddr*)&client, &len);
    printf("Client accepted...\n");
    i = 1;
    while(1)
    {
        recv(sock2, buf, 100, 0);
        sscanf(buf, "%s", command);
        if(!strcmp(command, "ls"))
        {
            system("ls >temps.txt");
            i = 0;
            stat("temps.txt",&obj);
            size = obj.st_size;
            send(sock2, &size, sizeof(int),0);
            filehandle = open("temps.txt", O_RDONLY);
            sendfile(sock2,filehandle,NULL,size);
        }
        else if(!strcmp(command,"get"))
        {
            sscanf(buf, "%s%s", filename, filename);
            stat(filename, &obj);
            filehandle = open(filename, O_RDONLY);
            size = obj.st_size;
            if(filehandle == -1)
                size = 0;
            send(sock2, &size, sizeof(int), 0);
            if(size)
                sendfile(sock2, filehandle, NULL, size);

        }
        else if(!strcmp(command, "put"))
        {
            int c = 0, len;
            char *f;
            sscanf(buf+strlen(command), "%s", filename);
            recv(sock2, &size, sizeof(int), 0);
            i = 1;
            while(1)
            {
                filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
                if(filehandle == -1)
                {
                    sprintf(filename + strlen(filename), "%d", i);
                }
                else
                    break;
            }
            f = malloc(size);
            recv(sock2, f, size, 0);
            c = write(filehandle, f, size);
            close(filehandle);
            send(sock2, &c, sizeof(int), 0);
        }
        else if(!strcmp(command, "pwd"))
        {
            system("pwd>temp.txt");
            i = 0;
            FILE*f = fopen("temp.txt","r");
            while(!feof(f))
                buf[i++] = fgetc(f);
            buf[i-1] = '\0';
            fclose(f);
            send(sock2, buf, 100, 0);
        }
        else if(!strcmp(command, "cd"))
        {
            if(chdir(buf+3) == 0)
                c = 1;
            else
                c = 0;
            send(sock2, &c, sizeof(int), 0);
        }


        else if(!strcmp(command, "bye") || !strcmp(command, "quit"))
        {
            printf("FTP server quitting..\n");
            i = 1;
            send(sock2, &i, sizeof(int), 0);
            exit(0);
        }
    }
}