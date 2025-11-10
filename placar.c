#include "placar.h"
#include <string.h> 

char placarEliminacao[MAX_JOGADORES][TAMANHO_NOME];
int placarIndex = 0;

void adicionarAoPlacar(const char* nome) {
    if (placarIndex < MAX_JOGADORES) {
        strcpy(placarEliminacao[placarIndex], nome);
        placarIndex++;
    }
}

void insertionSortPlacar(void) {
    int i, j;
    char chave[TAMANHO_NOME];
    
    int n = placarIndex - 1; 
    
    for (i = 1; i < n; i++) {
        strcpy(chave, placarEliminacao[i]);
        j = i - 1;
        while (j >= 0 && strcmp(placarEliminacao[j], chave) > 0) {
            strcpy(placarEliminacao[j + 1], placarEliminacao[j]);
            j = j - 1;
        }
        strcpy(placarEliminacao[j + 1], chave);
    }
}

void resetarPlacar(void) {
    placarIndex = 0;
    for (int i = 0; i < MAX_JOGADORES; i++) {
        placarEliminacao[i][0] = '\0';
    }
}