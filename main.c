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
} DOWNLOADER;

void showListOfURL(SP * spolData);
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
    pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t zober = PTHREAD_COND_INITIALIZER;
    SP spolData = {adresy, n, 0, false, "./",&mut, &zober};

    bool pokracuj = true;
    int decision = 0;
    printf("Communicator running\n");
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
                launcher(spolData);
                break;
            case 2:
                if (spolData.aktualPocet == spolData.maxPocet)
                {
                    printf("Poradovnik je plny.");
                } else {
                    addURL(&spolData);
                }
                break;
            case 3:
                directoryControl(&spolData);
                break;
            case 4:
                // TODO solve the problem of modifying history (probably create a structure from existing txt and then save it back)
                break;
            case 5:
                printf("Ukoncujem program...\n");
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
