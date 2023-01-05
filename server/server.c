#include "server.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

#define CMD_SIZE 100
#define MAXFILE 100
#define FILENAME 100

/*FTP server*/
/*for getting file size using stat()*/
#include<sys/stat.h>

/*for sendfile()*/
#include<sys/sendfile.h>

/*for O_RDONLY*/
#include<fcntl.h>

void tcpProtokol(int port);
void ftpProtokol(int port);
void *ConnectionHandler(void *socket_desc);
char* GetArgumentFromRequest(char* request);
bool SendFileOverSocket(int socket_desc, char* file_name);

int main()
{
    int end = 0;
    int port;
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
                port = 1025;
                tcpProtokol(port);
                end = 1;
                break;
            case 2:
                //printf("FTP server is not avaliable right now. Choose another one.\n");
                printf("You chose FTP server\n");
                port = 1026;
                ftpProtokol(port);
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


//FTP protokol metody
void ftpProtokol(int port)
{
    /*
    if(argc!=2){
        printf("Invalid arguments\n");
        return 0;
    }
    */

    int socket_desc, socket_client, *new_sock,
            c = sizeof(struct sockaddr_in);

    struct  sockaddr_in	server, client;

    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        perror("Could not create socket");
        return;
    }
    int SERVER_PORT = htons(port);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(SERVER_PORT);

    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Bind failed");
        return;
    }

    listen(socket_desc, 3);

    while (socket_client = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c))
    {
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = socket_client;
        pthread_create(&sniffer_thread, NULL, ConnectionHandler, (void*) new_sock);
        pthread_join(sniffer_thread, NULL);
    }

    if (socket_client<0)
    {
        perror("Accept failed");
        return;
    }

    return;
}

int GetCommandFromRequest(char* request)
{
    char cmd[CMD_SIZE];
    strcpy(cmd, request);
    int i = 0;
    while(request[i] != ' ' && request[i] != '\0')
        i++;
    if(request[i] == '\0')
        return 6;
    else
    {
        strncpy(cmd, request, i-1);
        cmd[i] = '\0';
    }

    if(!strcmp(cmd, "GET"))
        return 1;
    else if(!strcmp(cmd, "PUT"))
        return 2;
    else if(!strcmp(cmd, "MGET"))
        return 3;
    else if(!strcmp(cmd, "MPUT"))
        return 4;
    else if(!strcmp(cmd, "SHOW"))
        return 5;
    else if(!strcmp(cmd, "EXIT"))
        return 6;
    return 0;
}

void performGET(char *file_name, int socket)
{
    char server_response[BUFSIZ];
    printf("Performing GET request of client\n");

    // Check if file present
    if (access(file_name, F_OK) != -1)
    {
        //File is present on server
        //Send "OK" message
        strcpy(server_response, "OK");
        write(socket, server_response, strlen(server_response));

        //Send File
        SendFileOverSocket(socket, file_name);
    }
    else
    {

        printf("File not present at server.ABORTING.\n");

        // Requested file does not exist, notify the client
        strcpy(server_response, "NO");
        write(socket, server_response, strlen(server_response));
    }
}

void performPUT(char *file_name, int socket)
{
    int c,r;
    printf("Performing PUT request of client\n");

    char server_response[BUFSIZ], client_response[BUFSIZ];
    if(access(file_name, F_OK) != -1)
    {
        // Notifing client that file is present at server
        strcpy(server_response, "FP");
        write(socket, server_response, strlen(server_response));

        // Getting the users choice to override or not
        r = recv(socket, client_response, BUFSIZ, 0);
        client_response[r]='\0';

        if(!strcmp(client_response, "N")){
            printf("User says don't overwrite\n");
            return;
        }
        printf("User says to overwrite the file.\n");

    }
    else
    {
        // Send acknowledgement "OK"
        strcpy(server_response, "OK");
        write(socket, server_response, strlen(server_response));
    }


    // Getting File

    int file_size;
    char *data;
    // Recieving file size and allocating memory
    recv(socket, &file_size, sizeof(int), 0);
    data = malloc(file_size+1);

    // Creating a new file, receiving and storing data in the file.
    FILE *fp = fopen(file_name, "w");
    r = recv(socket, data, file_size, 0);
    data[r] = '\0';
    printf("Size of file recieved is %d\n",r);
    r = fputs(data, fp);
    fclose(fp);
}

void *ConnectionHandler(void *socket_desc)
{
    int	choice, file_desc, file_size;
    int socket = *(int*)socket_desc;

    char reply[BUFSIZ], file_ext[BUFSIZ],server_response[BUFSIZ], client_request[BUFSIZ], file_name[BUFSIZ];
    char *data;
    while(1)
    {	printf("\nWaiting for command\n");
        int l = recv(socket, client_request, BUFSIZ, 0);
        client_request[l]='\0';
        printf("Command Recieved %s\n",client_request );
        choice = GetCommandFromRequest(client_request);
        switch(choice)
        {
            case 1:
                strcpy(file_name, GetArgumentFromRequest(client_request));
                performGET(file_name, socket);
                break;
            case 2:
                strcpy(file_name, GetArgumentFromRequest(client_request));
                performPUT(file_name, socket);
                break;
            case 3:
                //strcpy(file_ext, GetArgumentFromRequest(client_request));
                //performMGET(socket,file_ext);
                break;
            case 4:
                //	performMPUT(socket);
                break;
            case 5:
                // showFile(socket);
                break;
            case 6:
                free(socket_desc);
                return 0;
        }
    }
    free(socket_desc);
    return 0;
}

char* GetArgumentFromRequest(char* request)
{
    char *arg = strchr(request, ' ');
    return arg + 1;
}

bool SendFileOverSocket(int socket_desc, char* file_name)
{
    struct stat	obj;
    int file_desc, file_size;

    printf("Sending File...\n");
    stat(file_name, &obj);

    // Open file
    file_desc = open(file_name, O_RDONLY);
    // Send file size
    file_size = obj.st_size;
    write(socket_desc, &file_size, sizeof(int));
    // Send File
    sendfile(socket_desc, file_desc, NULL, file_size);

    printf("File %s sent\n",file_name);
    return true;
}

/*
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
*/