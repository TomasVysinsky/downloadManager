#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
/*
#include "client/client.h"
#include "server/server.h"
*/

typedef struct url {
    char * address;
    int priority;
} URL;

typedef struct SpolData {
    URL * adresyNaStiahnutie;
    int maxPocet;
    int aktualPocet;
    pthread_mutex_t * mutex;
    pthread_cond_t * zober;
    //pthread_cond_t * urod;
} SP;

typedef struct downloader {
    int id;
    char * pridelenaAdresa;
    SP * data;
} DOWNLOADER;

typedef struct communicator {
    SP * data;
} COMMUNICATOR;

void * downloaderF(void * arg)
{
    DOWNLOADER * data = arg;
    printf("%d downloader running\n", data->id);
    return 1;
}

void * communicatorF(void * arg)
{
    COMMUNICATOR * data = arg;
    printf("Communicator running\n");
    return 1;
}

int main() {

    int n = 6;
    URL adresy[n];
    pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t zober = PTHREAD_COND_INITIALIZER;
    SP spolData = {adresy, n, 0, &mut, &zober};

    pthread_t downloaders[n];
    DOWNLOADER downloadersD[n];
    pthread_t communicator;
    COMMUNICATOR communicatorD = { &spolData };
    pthread_create(&communicator, NULL, communicatorF, &communicatorD);

    for (int i = 0; i < n; ++i) {
        downloadersD[i].id = i + 1;
        downloadersD[i].pridelenaAdresa = NULL;
        downloadersD[i].data = &spolData;
        pthread_create(&downloaders[i], NULL, downloaderF, &downloadersD[i]);
    }

    for (int i = 0; i < n; ++i) {
        pthread_join(downloaders[i],NULL);
    }
    pthread_join(communicator,NULL);

    pthread_mutex_destroy(&mut);
    pthread_cond_destroy(&zober);

    return 0;
    /*
    int koniec = 0;
    while (!koniec) {
        int programNaSpustenie = 0;
        printf("Vyberte program, ktory si zelate spustit:\n");
        printf(" 1) client\n");
        printf(" 2) server\n");
        scanf("%d", &programNaSpustenie);

        printf("\nLaunching application:\n");
        if (programNaSpustenie == 1) {
            printf("Client\n");
            mainClient("localhost");
            koniec = 1;
        } else if (programNaSpustenie == 2) {
            printf("Server\n");
            mainServer(1025);
            koniec = 1;
        } else {
            printf("Vyberte si z ponukanych moznosti \n\n");
        }
    }
    printf("Server a klient boli od hlavneho programu oddeleni.");
    return 0;*/
}
