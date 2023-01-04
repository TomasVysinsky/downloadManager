#include <stdio.h>
#include "client/client.h"
#include "server/server.h"

int main() {
    int programNaSpustenie = 1;
    printf("Vyberte program, ktory si zelate spustit: \n 1) client\n 2) server\n");
    scanf("%d", &programNaSpustenie);

    //TODO treba doplnit komunikaciu o vzbranom programe a blbuvzdornost voci zlym cislam
    if (programNaSpustenie == 1) {
        mainClient();
    } else {
        mainServer(1025);
    }
    return 0;
}
