// placar.c
#include "placar.h"
#include <string.h> 

PlacarEntry placarEliminacao[MAX_JOGADORES];
int placarIndex = 0;

void adicionarAoPlacar(const char* nome, float pontuacao) {
    if (placarIndex < MAX_JOGADORES) {
        strcpy(placarEliminacao[placarIndex].nome, nome);
        placarEliminacao[placarIndex].pontuacao = pontuacao; 
        placarIndex++;
    }
}

void insertionSortPlacar(void) {
    int i, j;
    PlacarEntry chave; 
    
    int n = placarIndex; 

    for (i = 1; i < n; i++) {
        chave = placarEliminacao[i]; 
        j = i - 1;

        while (j >= 0 && placarEliminacao[j].pontuacao < chave.pontuacao) {
            placarEliminacao[j + 1] = placarEliminacao[j]; 
            j = j - 1;
        }
        placarEliminacao[j + 1] = chave; 
    }
}

void resetarPlacar(void) {
    placarIndex = 0;
    for (int i = 0; i < MAX_JOGADORES; i++) {
        placarEliminacao[i].nome[0] = '\0';
        placarEliminacao[i].pontuacao = 0;
    }
}