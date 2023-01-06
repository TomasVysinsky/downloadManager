#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

typedef struct url {
    char * address;
    int priority;
} URL;

typedef struct SpolData {
    URL * adresyNaStiahnutie;
    int maxPocet;
    int aktualPocet;
    bool jeKoniec;
    pthread_mutex_t * mutex;
    pthread_cond_t * zapisuj;
} SP;

typedef struct downloader {
    int id;
    char * pridelenaAdresa;
    SP * data;
} DOWNLOADER;

void postponer();

void * downloaderF(void * arg)
{
    DOWNLOADER * dataD = arg;
    bool pokracuj = true;
    printf("%d downloader running\n", dataD->id);

    while (pokracuj)
    {
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

        // TODO metoda vyberania URL adries z poradovnika

        dataD->pridelenaAdresa = "https://www.tutorialspoint.com";
        //decision aky protokol pouzit
        char * hlavicka;
        if ((hlavicka = strstr(dataD->pridelenaAdresa, "https://")) != NULL) {
            printf("HTTPS\n");

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

        pthread_mutex_unlock(dataD->data->mutex);
    }

    printf("%d downloader ending\n", dataD->id);
    return NULL;
}

void launcher(SP spolData)
{
    int decision = 0;
    printf("Prajete si naplanovat cas spustenia?\n");
    printf(" 1) Ano\n");
    printf(" 2) Nie\n");
    scanf("%d", &decision);
    if (decision == 1)
        postponer();

    printf("\nZaciatok stahovania\n\n");

    int n = spolData.aktualPocet;
    pthread_t downloaders[n];
    DOWNLOADER downloadersD[n];
    for (int i = 0; i < n; ++i) {
        downloadersD[i].id = i + 1;
        downloadersD[i].pridelenaAdresa = NULL;
        downloadersD[i].data = &spolData;
        pthread_create(&downloaders[i], NULL, downloaderF, &downloadersD[i]);
    }


    for (int i = 0; i < n; ++i) {
        pthread_join(downloaders[i],NULL);
    }

    printf("\nStahovanie skoncilo\n\n");
}



int main() {

    int n = 10;
    URL adresy[n];
    pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t zober = PTHREAD_COND_INITIALIZER;
    SP spolData = {adresy, n, 0, false, &mut, &zober};



    bool pokracuj = true;
    int decision = 0;
    printf("Communicator running\n");
    while (pokracuj)
    {
        printf("Zvolte akciu, ktoru si prajete vykonat:\n");
        printf(" 1) Spustenie stahovania\n");
        printf(" 2) Pridanie suboru na stahovanie\n");
        printf(" 3) Nastavenie adresara\n");
        printf(" 4) Historia\n");
        printf(" 5) Ukoncit program\n");
        scanf("%d", &decision);
        switch (decision) {
            case 1:
                spolData.aktualPocet = 6;
                launcher(spolData);
                break;
            case 2:
                // TODO pridanie novej URL do zoznamu potrebnych URL
                break;
            case 3:
                // TODO UI
                    // TODO ls
                    // TODO cd
                    // TODO mkdir
                break;
            case 4:
                // TODO solve the problem of modifying history (probably create a structure from existing txt and then save it back)
                break;
            case 5:
                printf("Ukoncujem program...\n");
                /*pthread_mutex_lock(spolData.mutex);
                printf("Main mutex\n");
                spolData.jeKoniec = true;
                pthread_cond_broadcast(spolData.zapisuj);
                printf("Main broadcast\n");
                pthread_mutex_unlock(spolData.mutex);
                printf("Main mutex unlock\n");*/
                pokracuj = false;
                break;
            default:
                printf("\nZvolte jednu z ponukanych moznosti\n\n");
        }
    }

    pthread_mutex_destroy(&mut);
    pthread_cond_destroy(&zober);

    return 0;
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