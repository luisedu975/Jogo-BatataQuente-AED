#ifndef PLACAR_H
#define PLACAR_H

#include "utils.h" 

typedef struct {
    char nome[TAMANHO_NOME];
    float pontuacao;
} PlacarEntry;

extern PlacarEntry placarEliminacao[MAX_JOGADORES];
extern int placarIndex;

void adicionarAoPlacar(const char* nome, float pontuacao);
void insertionSortPlacar(void);
void resetarPlacar(void);

#endif