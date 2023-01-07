#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include<dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct url {
    char * address;
    int priority;
} URL;

typedef struct historyNode {
    int id;
    char * address;
    char * date;
    char * time;
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
    bool jeKoniec;
    char directory[128];
    pthread_mutex_t * mutex;
    pthread_cond_t * zapisuj;
} SP;

typedef struct downloader {
    int id;
    char * pridelenaAdresa;
    SP * data;
    HISTORY * history;
} DOWNLOADER;

void showListOfURL(SP * spolData);
void postponer();

void * downloaderF(void * arg)
{
    DOWNLOADER * dataD = arg;
    bool pokracuj = true;
    printf("%d downloader running\n", dataD->id);


    pthread_mutex_lock(dataD->data->mutex);
    printf("%d downloader mutex\n", dataD->id);
    while (dataD->data->aktualPocet <= 0 && !dataD->data->jeKoniec)
    {
        printf("%d downloader waiting\n", dataD->id);
        pthread_cond_wait(dataD->data->zapisuj, dataD->data->mutex);
        printf("%d downloader running\n", dataD->id);
    }
    if (dataD->data->jeKoniec)
    {
        printf("%d downloader is end\n", dataD->id);
        pokracuj = false;
    }

    if (dataD->data->aktualPocet == 0)
    {
        printf("%d downloader found an empty address queue. Ending...\n", dataD->id);
        return NULL;
    }
    dataD->data->aktualPocet--;
    dataD->pridelenaAdresa = dataD->data->adresyNaStiahnutie[dataD->data->aktualPocet].address;

    pthread_mutex_unlock(dataD->data->mutex);

    //dataD->pridelenaAdresa = "https://www.gnu.org/graphics/gnu-and-penguin-color-300x276.jpg";
    //http://speedtest.tele2.net/1MB.zip
    //https://www.gnu.org/graphics/gnu-and-penguin-color-300x276.jpg

    /* Extract the name of the file from URL */
    char * filename;
    filename = strrchr(dataD->pridelenaAdresa, '/');
    if (filename != NULL) {
        filename++; /* step over the slash */
    }
    //decision aky protokol pouzit
    char * hlavicka;
    if ((hlavicka = strstr(dataD->pridelenaAdresa, "https://")) != NULL) {
        printf("HTTPS\n");

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

            printf("%s\n", filename);
            file = fopen(filename, "wb");
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

            /* Include detecting HTTPS errors */
            curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

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
            return 1;
        }
    } else if ((hlavicka = strstr(dataD->pridelenaAdresa, "http://")) != NULL) {
        printf("HTTP\n");
    } else if ((hlavicka = strstr(dataD->pridelenaAdresa, "ftps://")) != NULL) {
        printf("FTPS\n");
    } else if ((hlavicka = strstr(dataD->pridelenaAdresa, "ftp://")) != NULL) {
        printf("FTP\n");
    } else {
        printf("Nepodporovana adresa!! (priklad: https://www.priklad.com)\n");
    }
    //implementovane protokoly



    // Zapis historie
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char date[50];
    sprintf(date, "%d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    char time[50];
    sprintf(date, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);

    // TODO zapisovanie do historie
    pthread_mutex_lock(dataD->history->mutex);
    dataD->history->nody[dataD->history->aktualPocet].id = dataD->history->aktualPocet + 1;
    dataD->history->nody[dataD->history->aktualPocet].address = dataD->pridelenaAdresa;
    dataD->history->nody[dataD->history->aktualPocet].date = date;
    dataD->history->nody[dataD->history->aktualPocet].time = time;
    dataD->history->aktualPocet++;
    pthread_mutex_unlock(dataD->history->mutex);

    printf("%d downloader ending\n", dataD->id);
    return NULL;
}

void launcher(SP *spolData, HISTORY *history)
{
    int decision = 0;
    printf("Prajete si naplanovat cas spustenia?\n");
    printf(" 1) Ano\n");
    printf(" 2) Nie\n");
    scanf("%d", &decision);
    if (decision == 1)
        postponer();

    printf("\nZaciatok stahovania\n\n");

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
    char* newURL;
    int priority = 0;
    printf("Zadajte pozadovanu URL adresu:\n");
    scanf("%s", &newURL);

    printf("Zadajte prioritu pre novu URL adresu: \n(0 a viac, cim vacsia tym nizsia priorita)\n");
    scanf("%d", &priority);

    // Algoritmus co zaradi novu URL na prislusne miesto podla priority tak, aby prvky s najvyssou prioritou ostali na konci
    URL cur = { newURL, priority };
    for (int i = 0; i <= spolData->aktualPocet; ++i) {
        if (i == spolData->aktualPocet)
        {
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
        printf(" 4) Zmaz priecinok\n"); //rm -r
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
                printf("Zadajte nazov suboru, ktory chcete vytvorit:\n");
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
                printf("Zadajte nazov suboru, ktory chcete vytvorit:\n");
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

int main() {

    int n = 10;
    URL adresy[n];
    pthread_mutex_t mutADD = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t mutHIS = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t zober = PTHREAD_COND_INITIALIZER;
    SP spolData = {adresy, n, 0, false, "./", &mutADD, &zober};

    // processing of time for the future needs
    /*
    time_t t;
    time(&t);
    printf("%s\n\n", ctime(&t));
    *//*
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("now: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    */


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
        count--;
        h += count;
        rewind(file);
    }
    HN nody[h];

    // Nacitanie Historie do nodov a modulu
    if (file != NULL) {
        int index = 0;
        char * address;
        fscanf(file, "%s\n", &address);
        int id;
        char * date;
        char * time;
        while (fscanf(file, "%d %s %s %s \n", &id, &address, &date, &time) == 1)
        {
            nody[index].id = id;
            nody[index].address = address;
            nody[index].date = date;
            nody[index].time = time;
            index++;
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
                if (history.aktualPocet == 0)
                {
                    printf("Momentalne sa tu nenachadza ziaden historicky zaznam.\n");
                } else {
                    for (int i = 0; i < history.aktualPocet; ++i) {

                        printf("%d %s %s %s \n", history.nody[i].id, history.nody[i].address, history.nody[i].date, history.nody[i].time);
                    }
                }
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

    char * tmp = "History";
    fprintf(file, "%s\n", tmp);
    tmp = " \n";
    for (int i = 0; i < history.aktualPocet; ++i) {
        fprintf(file, "%d %s %s %s \n", history.nody[i].id, history.nody[i].address, history.nody[i].date, history.nody[i].time);
    }
    fclose(file);

    pthread_mutex_destroy(&mutADD);
    pthread_mutex_destroy(&mutHIS);
    pthread_cond_destroy(&zober);

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
