#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <dirent.h>
#include <stdio.h>
#include <string.h>

#define MAXFILE 100
#define FILENAME 100

/*FTP Client*/
/*for getting file size using stat()*/
#include<sys/stat.h>

/*for sendfile()*/
#include<sys/sendfile.h>

/*for O_RDONLY*/
#include<fcntl.h>

void tcpClient(char * hostname);
void ftpClient(char * hostname, int port);

void performGET(char *file_name,int socket_desc);
void performPUT(char *file_name,int socket_desc);
int SendFileOverSocket(int socket_desc, char* file_name);
void performMGET(int server_socket);
void performMPUT(int server_socket);

int main()
{
    int end = 0;
    printf("Welcome to Client\n");
    while (!end)
    {
        printf("Choose type of client you want to launch:\n");
        printf(" 1) TCP\n");
        printf(" 2) FTP\n");
        printf(" 3) FTPS\n");
        printf(" 4) HTTP\n");
        printf(" 5) HTTPS\n");
        int decision = 0;
        scanf("%d", &decision);

        switch (decision) {
            case 1:
                printf("You chose TCP client\n");
                tcpClient("localhost");
                end = 1;
                break;
            case 2:
                //printf("FTP server is not avaliable right now. Choose another one.\n");
                printf("You chose FTP client\n");
                ftpClient("localhost", 1026);
                end = 1;
                break;
            case 3:
                printf("FTPS server is not avaliable right now. Choose another one.\n");
                break;
            case 4:
                printf("HTTP server is not avaliable right now. Choose another one.\n");
                break;
            case 5:
                printf("HTTPS server is not avaliable right now. Choose another one.\n");
                break;
            default:
                printf("Chose from the avaliable options.\n\n");
        }
    }
}

void tcpClient(char * hostname)
{

    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent* server;

    char buffer[256];

//    if (argc < 3)
//    {
//        fprintf(stderr,"usage %s hostname port\n", argv[0]);
//        return 1;
//    }

    server = gethostbyname(hostname);
    if (server == NULL)
    {
        fprintf(stderr, "Error, no such host\n");
        return;
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(
            (char*)server->h_addr,
            (char*)&serv_addr.sin_addr.s_addr,
            server->h_length
    );
    serv_addr.sin_port = htons(1025);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
        return;
    }

    if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error connecting to socket");
        return;
    }

    printf("Please enter a message: ");
    bzero(buffer,256);
    fgets(buffer, 255, stdin);

    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
        perror("Error writing to socket");
        return;
    }

    bzero(buffer,256);
    n = read(sockfd, buffer, 255);
    if (n < 0)
    {
        perror("Error reading from socket");
        return;
    }

    printf("%s\n",buffer);
    close(sockfd);
}

void ftpClient(char * hostname, int port)
{
//    if(argc!=3){
//        printf("Invalid arguments\n");
//        return 0;
//    }
    int socket_desc;
    struct sockaddr_in server;
    char request_msg[BUFSIZ], reply_msg[BUFSIZ], file_name[BUFSIZ];

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        perror("Could not create socket");
        return;
    }

    char SERVER_IP[BUFSIZ];
    int SERVER_PORT;
    strcpy(SERVER_IP,hostname);
    SERVER_PORT=htons(port);
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);

    // Connect to server
    if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Connection failed");
        return;
    }

    int choice = 0;
    while(1)
    {
        printf("Enter a choice:\n1- GET\n2- PUT\n3- MGET\n4- MPUT\n5- EXIT\n");
        scanf("%d", &choice);
        switch(choice)
        {
            case 1:
                printf("Enter file_name to get: ");
                scanf("%s", file_name);
                performGET(file_name,socket_desc);
                break;
            case 2:
                printf("Enter file_name to put: ");
                scanf("%s", file_name);
                performPUT(file_name,socket_desc);
                break;
            case 3:
                performMGET(socket_desc);
                break;
            case 4:
                performMPUT(socket_desc);
                break;
            case 5:
                strcpy(request_msg,"EXIT");
                write(socket_desc, request_msg, strlen(request_msg));
                return;
            default:
                printf("Incorrect command\n");
        }
    }
}

int SendFileOverSocket(int socket_desc, char* file_name)
{
    struct stat	obj;
    int file_desc, file_size;

    stat(file_name, &obj);
    file_desc = open(file_name, O_RDONLY);
    file_size = obj.st_size;
    send(socket_desc, &file_size, sizeof(int), 0);
    sendfile(socket_desc, file_desc, NULL, file_size);

    return 1;
}

void performGET(char *file_name,int socket_desc){
    char request_msg[BUFSIZ], reply_msg[BUFSIZ];
    int file_size;
    char *data;
    int t;
    if( access( file_name, F_OK ) != -1 )
    {
        int abortflag = 0;
        printf("File already exists locally. Press 1 to overwrite. Press any other key to abort.\n");
        scanf("%d", &abortflag);
        if(abortflag!=1)
            return;
    }
    // Get a file from server
    strcpy(request_msg, "GET ");
    strcat(request_msg, file_name);
    write(socket_desc, request_msg, strlen(request_msg));
    recv(socket_desc, reply_msg, 2, 0);
    reply_msg[2] = '\0';
    printf("%s\n", reply_msg);
    if (strcmp(reply_msg, "OK") == 0)
    {

        // File present at server.Start receiving the file and storing locally.
        printf("Recieving data\n");

        recv(socket_desc, &file_size, sizeof(int), 0);
        data = malloc(file_size+1);
        FILE *fp = fopen(file_name, "w");
        t = recv(socket_desc, data, file_size, 0);
        data[t] = '\0';
        fputs(data, fp);
        fclose(fp);
        printf("File %s recieved with size %d \n", file_name,t);
    }
    else
    {
        printf("File doesn't exist at server.ABORTING.\n");
    }
}

void performPUT(char *file_name,int socket_desc)
{
    int	file_size, file_desc,c,t;
    char *data;
    char request_msg[BUFSIZ], reply_msg[BUFSIZ],client_response[2];
    // Get a file from server
    strcpy(request_msg, "PUT ");
    strcat(request_msg, file_name);
    printf("Trying to PUT %s to server. \n",file_name );
    if (access(file_name, F_OK) != -1)
    {
        // Sending PUT request to server.
        write(socket_desc, request_msg, strlen(request_msg));
        t = recv(socket_desc, reply_msg, BUFSIZ, 0);
        reply_msg[t]='\0';
        if (strcmp(reply_msg, "OK") == 0)
        {
            // Everything if fine and send file
            SendFileOverSocket(socket_desc, file_name);
        }
        else if(strcmp(reply_msg, "FP") == 0)
        {
            // File present at server.
            printf("File exists in server. Do you wan't to overwrite? 1/0\n");
            scanf("%d", &c);
            if(c)
            {
                // User says yes to overwrite. Send Y and then data
                printf("Overwriting %s\n",file_name );
                strcpy(client_response, "Y");
                write(socket_desc, client_response, strlen(client_response));
                SendFileOverSocket(socket_desc, file_name);
            }
            else
            {
                printf("Not sending %s file to server\n",file_name);
                // User says No to overwrite. Send N and exit
                strcpy(client_response, "N");
                write(socket_desc, client_response, strlen(client_response));
                return;
            }
        }
        else{
            // Server can't create file.
            printf("Server can't create file...\n");
        }
    }
    else
    {
        // File not found locally hence abort.
        printf("File not found locally...\n");
        return;
    }
}

void performMGET(int socket_desc){
    printf("Performing MGET\n");

    char ext[BUFSIZ],request_msg[BUFSIZ],file_name[BUFSIZ];
    char *data;
    int file_size;
    char reply[BUFSIZ];
    int r;
    printf("Type Extension :\n");
    fflush(stdout);
    scanf("%s",ext);

    //Send Server Command to get all files with given extension
    strcpy(request_msg,"MGET ");
    strcat(request_msg,ext);
    int l = write(socket_desc, request_msg, strlen(request_msg));
    int file_name_size=0;
    while(1){
        int t = recv(socket_desc,file_name,BUFSIZ,0);
        file_name[t]='\0';
        if(strcmp(file_name,"END")==0)
            break;
        printf("Recieving %s\n",file_name);

        // If file present at server ask user for overwrite or not.
        if( access( file_name, F_OK ) != -1 )
        {
            int abortflag = 0;
            printf("File already exists. Press 1 to overwrite. Press any other key to abort.\n");
            scanf("%d", &abortflag);
            if(abortflag!=1){
                // Send "NO" is user doesn't want to overwrite
                strcpy(reply,"NO");
                write(socket_desc,reply,2);
                printf("Not Overwriting %s \n",file_name);
                continue;
            }
            printf("Overwriting %s\n",file_name );
        }
        // Send "OK" if user wantes to overwrite
        strcpy(reply,"OK");
        write(socket_desc,reply,2);

        // Checking if file present at server.This would be "OK" only
        recv(socket_desc, reply, 2, 0);
        if (strcmp(reply, "OK") == 0)
        {

            // Recieving file size, creating file, getting data and writing in file.
            recv(socket_desc, &file_size, sizeof(int), 0);
            data = malloc(file_size+1);
            FILE *fp = fopen(file_name, "w");
            r = recv(socket_desc, data, file_size, 0);
            data[r] = '\0';
            fputs(data, fp);
            fclose(fp);
            printf("File %s received with size %d\n", file_name,r);
        }
        else
            printf("This shouldn't have happen as we are sure file is present at server.\n" );

    }
    printf("MGET Complete\n");

}

void performMPUT(int server_socket){
    printf("Performing MPUT\n");

    char ext[BUFSIZ],request_msg[BUFSIZ];
    printf("Type Extension\n");
    scanf("%s",ext);

    DIR *d;
    char *p1,*p2;
    int ret;
    struct dirent *dir;
    d = opendir(".");
    char full_name[100];
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            strcpy(full_name,dir->d_name);

            p1=strtok(dir->d_name,".");
            p2=strtok(NULL,".");
            if(p2!=NULL && strcmp(p2,ext)==0)
                performPUT(full_name,server_socket);
        }
        closedir(d);
    }
    printf("MPUT Complete\n");

}

/*
void ftpClient(char * hostname)
{
    //TODO skontrolovat ako funguje tato metoda
    struct sockaddr_in server;
    struct stat obj;
    int sock;
    int choice;
    char buf[100], command[5], filename[20], *f;
    int k, size, status;
    int filehandle;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1)
    {
        printf("socket creation failed");
        exit(1);
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(1026);
    server.sin_addr.s_addr = INADDR_ANY;
    k = connect(sock,(struct sockaddr*)&server, sizeof(server));
    if(k == -1)
    {
        printf("Connect Error");
        exit(1);
    }
    int i = 1;
    while(1) {
        printf("Enter a choice:\n1- get\n2- put\n3- pwd\n4- ls\n5- cd\n6- quit\n");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                printf("Enter filename to get: ");
                scanf("%s", filename);
                strcpy(buf, "get ");
                strcat(buf, filename);
                send(sock, buf, 100, 0);
                recv(sock, &size, sizeof(int), 0);
                if (!size) {
                    printf("No such file on the remote directory\n\n");
                    break;
                }
                f = malloc(size);
                recv(sock, f, size, 0);
                while (1) {
                    filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
                    if (filehandle == -1) {
                        sprintf(filename + strlen(filename), "%d",
                                i);//needed only if same directory is used for both server and client
                    } else break;
                }
                //write(filehandle, f, size, 0);
                write(filehandle, f, size);
                close(filehandle);
                strcpy(buf, "cat ");
                strcat(buf, filename);
                system(buf);
                break;
            case 2:
                // TODO Upload netreba?

                printf("Enter filename to put to server: ");
                scanf("%s", filename);
                filehandle = open(filename, O_RDONLY);
                if (filehandle == -1) {
                    printf("No such file on the local directory\n\n");
                    break;
                }
                strcpy(buf, "put ");
                strcat(buf, filename);
                send(sock, buf, 100, 0);
                stat(filename, &obj);
                size = obj.st_size;
                send(sock, &size, sizeof(int), 0);
                sendfile(sock, filehandle, NULL, size);
                recv(sock, &status, sizeof(int), 0);
                if (status)
                    printf("File stored successfully\n");
                else
                    printf("File failed to be stored to remote machine\n");
                break;
            case 3:
                strcpy(buf, "pwd");
                send(sock, buf, 100, 0);
                recv(sock, buf, 100, 0);
                printf("The path of the remote directory is: %s\n", buf);
                break;
            case 4:
                strcpy(buf, "ls");
                send(sock, buf, 100, 0);
                recv(sock, &size, sizeof(int), 0);
                f = malloc(size);
                recv(sock, f, size, 0);
                filehandle = creat("temp.txt", O_WRONLY);
                //write(filehandle, f, size, 0);
                write(filehandle, f, size);
                close(filehandle);
                printf("The remote directory listing is as follows:\n");
                system("cat temp.txt");
                break;
            case 5:
                strcpy(buf, "cd ");
                printf("Enter the path to change the remote directory: ");
                scanf("%s", buf + 3);
                send(sock, buf, 100, 0);
                recv(sock, &status, sizeof(int), 0);
                if (status)
                    printf("Remote directory successfully changed\n");
                else
                    printf("Remote directory failed to change\n");
                break;
            case 6:
                strcpy(buf, "quit");
                send(sock, buf, 100, 0);
                recv(sock, &status, 100, 0);
                if (status) {
                    printf("Server closed\nQuitting..\n");
                    exit(0);
                }
                printf("Server failed to close connection\n");
        }
    }
}*/