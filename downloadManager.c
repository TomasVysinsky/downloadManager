#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFSIZE 1024
#define ADDSIZE 250

/* HTTPS links */
// https://www.gnu.org/graphics/gnu-and-penguin-color-300x276.jpg
// https://www.actualidadmotor.com/wp-content/uploads/2016/10/subaru-wrx-s4-ts-830x460.jpg
// https://image.pmgstatic.com/cache/resized/w420/files/images/film/posters/157/804/157804175_c17547.jpg
// https://www.attelier.sk/wp-content/uploads/2021/01/cbvvx-735x1024.jpg

/* HTTP links */
// http://speedtest.tele2.net/1MB.zip
// http://sstatic.net/stackexchange/img/logos/so/so-logo-med.png
// http://info.cern.ch/hypertext/Copyright.html

/* FTP link */
// ftp://vinidev.com/cukrarka.sql

typedef struct url {
    char address[ADDSIZE];
    int priority;
} URL;

typedef struct historyNode {
    int id;
    char address[ADDSIZE];
    char date[50];
    char time[50];
} HN;

typedef struct history {
    HN * nody;
    int maxPocet;
    int aktualPocet;
    pthread_mutex_t * mutex;
} HISTORY;

typedef struct SpolData {
    URL * adresyNaStiahnutie;
    int maxPocet;
    int aktualPocet;
    char directory[128];
    pthread_mutex_t * mutex;
} SP;

typedef struct downloader {
    int id;
    char * pridelenaAdresa;
    SP * data;
    HISTORY * history;
} DOWNLOADER;

void showListOfURL(SP * spolData);
void postponer();
int parseHeader(int sock);

void * downloaderF(void * arg)
{
    DOWNLOADER * dataD = arg;
    bool pokracuj = true;
    printf("%d downloader running\n", dataD->id);

    pthread_mutex_lock(dataD->data->mutex);
    printf("%d downloader mutex\n", dataD->id);

    if (dataD->data->aktualPocet == 0)
    {
        printf("%d downloader found an empty address queue. Ending...\n", dataD->id);
        return NULL;
    }
    dataD->data->aktualPocet--;
    dataD->pridelenaAdresa = dataD->data->adresyNaStiahnutie[dataD->data->aktualPocet].address;

    pthread_mutex_unlock(dataD->data->mutex);

    /* Extract the name of the file from URL */
    char * filename;
    filename = strrchr(dataD->pridelenaAdresa, '/');
    if (filename != NULL) {
        filename++;
    }
    //decision aky protokol pouzit
    char * hlavicka;
    if ((hlavicka = strstr(dataD->pridelenaAdresa, "https://")) != NULL) {
        CURL *curl;
        CURLcode result;
        FILE *file;

        curl = curl_easy_init();
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, dataD->pridelenaAdresa);

            /* Extract the name of the file from URL */
            filename = strrchr(dataD->pridelenaAdresa, '/');
            if (filename != NULL) {
                filename++; /* step over the slash */
            }

            char tmp[ADDSIZE];
            strcpy(tmp, filename);
            char destination[ADDSIZE];
            strcpy(destination, dataD->data->directory);
            strcat(destination, filename);
            printf("%s\n", destination);
            file = fopen(destination, "wb");
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

            /* Include detecting HTTPS errors */
            curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

            result = curl_easy_perform(curl);
            if (result != CURLE_OK)
            {
                fprintf(stderr, "ERROR curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
                printf("Address failed: %s\n", dataD->pridelenaAdresa);
            }

            fclose(file);

            curl_easy_cleanup(curl);
        } else {
            fprintf(stderr, "ERROR something went wrong initializing curl\n");
            return NULL;
        }
    } else if ((hlavicka = strstr(dataD->pridelenaAdresa, "http://")) != NULL) {
        int sockfd, port, n;
        struct sockaddr_in serv_addr;
        struct hostent *server;
        char buffer[BUFSIZE], url[250], hostname[64], path[128];
        FILE *file;

        port = 80;

        strcpy(url, dataD->pridelenaAdresa);
        sscanf(url, "http://%99[^/]%99[^\n]", hostname, path);

        server = gethostbyname(hostname);
        if (server == NULL) {
            fprintf(stderr,"ERROR, no such host as %s\n", dataD->pridelenaAdresa);
            return NULL;
        }

        /* Vytvor socket */
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("ERROR opening socket");
            return NULL;
        }

        /* Vynulujeme a zinicializujeme sietovu adresu. */
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr,
              (char *)&serv_addr.sin_addr.s_addr, server->h_length);
        serv_addr.sin_port = htons(port);

        /* Vytvorime spojenie so serverom */
        if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            perror("ERROR connecting to server");
            return NULL;
        }

        /* Odosli HTTP GET request */
        sprintf(buffer, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", path, hostname);
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) {
            perror("ERROR writing to socket after GET");
            return NULL;
        }

        char tmp[ADDSIZE];
        strcpy(tmp, filename);
        char destination[ADDSIZE];
        strcpy(destination, dataD->data->directory);
        strcat(destination, filename);
        printf("%s\n", destination);
        file = fopen(destination, "wb");
        if (file == NULL) {
            perror("ERROR opening file");
            return NULL;
        }

        int contentLength;
        if (contentLength = parseHeader(sockfd)) {
            int bytes = 0;
            bzero(buffer, BUFSIZE);
            while ((n = read(sockfd, buffer, BUFSIZE)) > 0) {
                if (n < 0) {
                    perror("ERROR reading from socket");
                    return NULL;
                }
                fwrite(buffer, 1, n, file);

                bytes += n;
                if (bytes==contentLength)
                    break;
            }

            fclose(file);
        }


        close(sockfd);
    } else if ((hlavicka = strstr(dataD->pridelenaAdresa, "ftps://")) != NULL) {
        CURL *curl;
        CURLcode result;
        FILE *file;

        curl = curl_easy_init();
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, dataD->pridelenaAdresa);
            curl_easy_setopt(curl, CURLOPT_USERNAME, "host");
            curl_easy_setopt(curl, CURLOPT_PASSWORD, "host");

            file = fopen(filename, "wb");
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

            /* Switch on full protocol/debug output */
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            /* Perform the request, res will get the return code */
            result = curl_easy_perform(curl);

            /* Check for errors */
            if (result != CURLE_OK)
                fprintf(stderr, "curl_easy_perform() failed: %s\n",
                        curl_easy_strerror(result));

            fclose(file);

            /* always cleanup */
            curl_easy_cleanup(curl);
        } else {
            fprintf(stderr, "Error: something went wrong initializing curl\n");
            return NULL;
        }
    } else if ((hlavicka = strstr(dataD->pridelenaAdresa, "ftp://")) != NULL) {
        CURL *curl;
        CURLcode result;
        FILE *file;

        curl = curl_easy_init();
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, dataD->pridelenaAdresa);
            curl_easy_setopt(curl, CURLOPT_USERNAME, "host");
            curl_easy_setopt(curl, CURLOPT_PASSWORD, "host");

            file = fopen(filename, "wb");
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

            /* Switch on full protocol/debug output */
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            /* Perform the request, res will get the return code */
            result = curl_easy_perform(curl);

            /* Check for errors */
            if (result != CURLE_OK)
                fprintf(stderr, "curl_easy_perform() failed: %s\n",
                        curl_easy_strerror(result));

            fclose(file);

            /* always cleanup */
            curl_easy_cleanup(curl);
        } else {
            fprintf(stderr, "Error: something went wrong initializing curl\n");
            return NULL;
        }
    } else {
        printf("Nepodporovana adresa!! (priklad: https://www.priklad.com)\n");
    }
    //implementovane protokoly



    // Zapis historie
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char date[50];
    sprintf(date, "%d-%02d-%02d", (tm.tm_year + 1900), tm.tm_mon + 1, tm.tm_mday);
    char time[50];
    sprintf(time, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);


    pthread_mutex_lock(dataD->history->mutex);
    dataD->history->nody[dataD->history->aktualPocet].id = dataD->history->aktualPocet + 1;
    strcpy(dataD->history->nody[dataD->history->aktualPocet].address, dataD->pridelenaAdresa);
    strcpy(dataD->history->nody[dataD->history->aktualPocet].date, date);
    strcpy(dataD->history->nody[dataD->history->aktualPocet].time, time);
    dataD->history->aktualPocet++;
    pthread_mutex_unlock(dataD->history->mutex);

    printf("%d downloader ending\n", dataD->id);
    return NULL;
}

void launcher(SP *spolData, HISTORY *history)
{
    int decision = 0;
    if (spolData->aktualPocet == 0)
    {
        printf("V zozname sa nenachadzaju ziadne adresy na stiahnutie\n");
        return;
    }
    if (history->aktualPocet + spolData->aktualPocet > history->maxPocet)
    {
        int h = history->maxPocet + 50;
        HN nody[h];
        for (int i = 0; i < history->aktualPocet; ++i) {
            nody[i] = history->nody[i];
        }
        history->nody = nody;
    }

    printf("Prajete si naplanovat cas spustenia?\n");
    printf(" 1) Ano\n");
    printf(" 2) Nie\n");
    scanf("%d", &decision);
    if (decision == 1)
        postponer();

    printf("\nZaciatok stahovania...\n");

    int n = spolData->aktualPocet;
    pthread_t downloaders[n];
    DOWNLOADER downloadersD[n];
    for (int i = 0; i < n; ++i) {
        downloadersD[i].id = i + 1;
        downloadersD[i].pridelenaAdresa = NULL;
        downloadersD[i].data = spolData;
        downloadersD[i].history = history;
        pthread_create(&downloaders[i], NULL, downloaderF, &downloadersD[i]);
    }


    for (int i = 0; i < n; ++i) {
        pthread_join(downloaders[i],NULL);
    }

    printf("\nStahovanie skoncilo\n\n");
}

void addURL(SP *spolData) {
    URL cur;
    printf("Zadajte pozadovanu URL adresu:\n");
    scanf("%s", cur.address);

    for (int i = 0; i < spolData->aktualPocet; ++i) {
        if (!strcmp(spolData->adresyNaStiahnutie[i].address, cur.address))
        {
            printf("The address is already in the queue\n");
            return;
        }
    }

    printf("Zadajte prioritu pre novu URL adresu: \n(0 a viac, cim vacsie cislo, tym nizsia priorita)\n");
    scanf("%d", &cur.priority);
    //printf("scan done\n");
    // Algoritmus co zaradi novu URL na prislusne miesto podla priority tak, aby prvky s najvyssou prioritou ostali na konci
    //printf("assign done\n");
    for (int i = 0; i <= spolData->aktualPocet; ++i) {
        if (i == spolData->aktualPocet)
        {
            //printf("got to the last decision\n");
            spolData->adresyNaStiahnutie[i] = cur;
        } else {
            if (cur.priority >= spolData->adresyNaStiahnutie[i].priority)
            {
                URL tmp = spolData->adresyNaStiahnutie[i];
                spolData->adresyNaStiahnutie[i] = cur;
                cur = tmp;
            }
        }
    }
    spolData->aktualPocet++;
    //printf("Pocet navyseny\n");
}

void directoryControl(SP *spolData) {
    bool pokracuj = true;
    int decision = 0;
    DIR *dp = NULL;
    char dir[128];
    char parameter[200];
    while (pokracuj) {
        printf("Zvolte akciu, ktoru si prajete vykonat:\n");
        printf(" 1) Ukaz aktualny priecinok\n"); //pwd
        printf(" 2) Zmen aktualny priecinok\n"); //cd
        printf(" 3) Vytvor priecinok\n"); //mkdir
        printf(" 4) Zmaz priecinok/subor\n"); //rm -r
        printf(" 5) Ukaz subory v aktualnom priecinku\n"); //ls
        printf(" 6) Navrat do hlavneho menu\n");
        scanf("%d", &decision);
        printf("\n");
        switch (decision) {
            case 1:
                printf("");
                char commandRL[20] = "readlink -f ";
                strcat(commandRL, spolData->directory);
                system(commandRL);
                break;
            case 2:
                printf("Zadajte cestu k priecinku (Priklad: /home/user):\n");
                scanf("%s", parameter);

                // Zabezpecenie aby sa na konci nachadzal /
                if(parameter[strlen(parameter)-1] != '/')
                {
                    strcat(parameter,"/");
                }

                if(NULL == (dp = opendir(parameter)) )
                {
                    printf("\n Zadana adresa neexistuje [%s]\n", parameter);
                } else {
                    strcpy(spolData->directory, parameter);
                    closedir(dp);
                }
                break;
            case 3:
                printf("Zadajte nazov priecinku, ktory chcete vytvorit:\n");
                scanf("%s", parameter);
                char commandMD[10] = "mkdir ";
                //chystanie adresy na vymazavanie
                strcpy(dir, spolData->directory);
                strcat(commandMD, dir);
                strcat(commandMD, parameter);
                //printf("%s\n", &commandRM);
                system(commandMD);
                break;
            case 4:
                printf("Zadajte subor alebo priecinok ktory chcete vymazat:\n");
                scanf("%s", parameter);
                char commandRM[10] = "rm -r ";
                //chystanie adresy na vymazavanie
                strcpy(dir, spolData->directory);
                strcat(commandRM, dir);
                strcat(commandRM, parameter);
                //printf("%s\n", &commandRM);
                system(commandRM);
                break;
            case 5:
                printf("");
                char commandLS[10] = "ls ";
                strcat(commandLS, spolData->directory);
                system(commandLS);
                break;
            case 6:
                pokracuj = false;
                break;
            default:
                printf("\nZvolte jednu z ponukanych moznosti\n");
        }
        printf("\n");
    }
}

void historyControl(HISTORY *history) {
    bool pokracuj = true;
    int decision = 0;
    char parameter[200];
    bool koniec = false;
    while (pokracuj) {
        printf("Zvolte akciu, ktoru si prajete vykonat:\n");
        printf(" 1) Ukaz historiu\n"); //pwd
        printf(" 2) Zmaz historiu\n");
        printf(" 3) Navrat do hlavneho menu\n");
        scanf("%d", &decision);
        printf("\n");
        switch (decision) {
            case 1:
                if (history->aktualPocet == 0)
                {
                    printf("Momentalne sa tu nenachadza ziaden historicky zaznam.\n");
                } else {
                    for (int i = 0; i < history->aktualPocet; ++i) {

                        printf("%d %s %s %s \n", history->nody[i].id, history->nody[i].address, history->nody[i].date, history->nody[i].time);
                    }
                }
                break;
            case 2:
                koniec = false;
                while (!koniec)
                {
                    printf("Skutocne chcete zmazat celu historiu?\n");
                    printf(" 1) Ano\n");
                    printf(" 2) Nie\n");
                    scanf("%d", &decision);
                    switch (decision) {
                        case 1:
                            printf("\nPremazavam historiu...\n");
                            history->aktualPocet = 0;
                            koniec = true;
                            break;
                        case 2:

                            koniec = true;
                            break;
                        default:
                            printf("Zadajte cislicu 1 alebo 2\n\n");
                    }
                }
                break;
            case 3:
                pokracuj = false;
                break;
            default:
                printf("\nZvolte jednu z ponukanych moznosti\n");
        }
        printf("\n");
    }
}

int main() {

    int n = 10;
    URL adresy[n];
    pthread_mutex_t mutADD = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t mutHIS = PTHREAD_MUTEX_INITIALIZER;
    SP spolData = {adresy, n, 0, "./", &mutADD};

    // Vznik modulu historie
    char filename[20] = "history.txt";
    int h = 50;
    int count = 0;
    FILE * file = fopen(filename, "r");
    // Cast kde sa podla toho ci history file existuje urci, aky velky bude modul historie
    if (file != NULL) {
        // history file exists
        char c;
        for (c = getc(file); c != EOF; c = getc(file))
            if (c == '\n') // Increment count if this character is newline
                count++;
        count;
        h += count;
        //rewind(file);
        fseek(file, 0, SEEK_SET);
    }
    HN nody[h];

    // Nacitanie Historie do nodov a modulu
    if (file != NULL) {
        char address[ADDSIZE];
        //fscanf(file, "%*s\n");
        int id;
        char date[50];
        char time[50];
        for (int i = 0; i < count; ++i)
        {
            fscanf(file, "%d %s %s %s \n", &id, address, date, time);
            nody[i].id = id;
            strcpy(nody[i].address, address);
            strcpy(nody[i].date, date);
            strcpy(nody[i].time, time);
        }
        fclose(file);
    }
    HISTORY history = {nody, h, count, &mutHIS};


    bool pokracuj = true;
    int decision = 0;
    //printf("Communicator running\n");
    while (pokracuj)
    {
        showListOfURL(&spolData);
        printf("Zvolte akciu, ktoru si prajete vykonat:\n");
        printf(" 1) Spustenie stahovania\n");
        printf(" 2) Pridanie suboru na stahovanie\n");
        printf(" 3) Nastavenie adresara\n");
        printf(" 4) Historia\n");
        printf(" 5) Ukoncit program\n");
        scanf("%d", &decision);
        printf("\n");
        switch (decision) {
            case 1:
                launcher(&spolData, &history);
                break;
            case 2:
                if (spolData.aktualPocet == spolData.maxPocet)
                {
                    printf("Poradovnik je plny.\n");
                } else {
                    addURL(&spolData);
                }
                break;
            case 3:
                directoryControl(&spolData);
                break;
            case 4:
                historyControl(&history);
                /*
                if (history.aktualPocet == 0)
                {
                    printf("Momentalne sa tu nenachadza ziaden historicky zaznam.\n");
                } else {
                    for (int i = 0; i < history.aktualPocet; ++i) {

                        printf("%d %s %s %s \n", history.nody[i].id, history.nody[i].address, history.nody[i].date, history.nody[i].time);
                    }
                }*/
                break;
            case 5:
                printf("Ukoncujem program...\n");
                pokracuj = false;
                break;
            default:
                printf("\nZvolte jednu z ponukanych moznosti\n\n");
        }
    }

    file = fopen(filename, "w");
    if (file == NULL)
    {
        fprintf(stderr, "\nError opened file\n");
        exit (1);
    }

    //char * tmp = "History";
    //fprintf(file, "%s\n", tmp);
    //tmp = " \n";
    for (int i = 0; i < history.aktualPocet; ++i) {
        fprintf(file, "%d %s %s %s \n", history.nody[i].id, history.nody[i].address, history.nody[i].date, history.nody[i].time);
    }
    fclose(file);

    pthread_mutex_destroy(&mutADD);
    pthread_mutex_destroy(&mutHIS);

    return 0;
}

void showListOfURL(SP * spolData){
    printf("\n\nAktualny zoznam URL:\n");
    if (spolData->aktualPocet == 0)
    {
        printf("V zozname sa nenachadzaju ziadne URL \n");
    } else {
        for (int i = 0; i < spolData->aktualPocet; ++i) {
            printf("Priorita: %d Stranka: %s\n", spolData->adresyNaStiahnutie[i].priority, &spolData->adresyNaStiahnutie[i].address);
        }
    }
    printf("\n\n");
}

void postponer()
{
    int minutes, seconds;
    printf("Zadajte kolko minut ma program cakat\n");
    scanf("%d", &minutes);
    printf("Zadajte kolko sekund ma program cakat\n");
    scanf("%d", &seconds);
    //int total = seconds + (minutes * 60);
    seconds += minutes * 60;

    printf("Odpocitavanie do zaciatku stahovania:\n");
    for (int i = 0; i < seconds; ++i) {
        printf("%d sekund\n", (seconds - i));
        sleep(1);
    }
}

int parseHeader(int sock) {
    char buffer[BUFSIZE] = "", *ptr = buffer + 4;
    int n; //num of bytes

    while (n = recv(sock, ptr, 1, 0)) {
        if(n == -1){
            perror("ERROR Parse Header");
            exit(1);
        }

        if( (ptr[-3] == '\r')  && (ptr[-2] == '\n' ) &&
            (ptr[-1] == '\r')  && (*ptr == '\n' ))
            break;

        ptr++;
    }

    *ptr = 0;
    ptr = buffer +4 ;

    if (n) {
        ptr = strstr(ptr,"Content-Length:");
        if (ptr) {
            sscanf(ptr,"%*s %d", &n);

        } else
            n = -1; //unknown size
    }

    return n;
}