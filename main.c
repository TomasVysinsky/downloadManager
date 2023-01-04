#include <stdio.h>
#include "client/client.h"
#include "server/server.h"

int main() {
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
    return 0;
}
