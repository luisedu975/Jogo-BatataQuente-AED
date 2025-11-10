#ifndef PLACAR_H
#define PLACAR_H

#include "utils.h" 

extern char placarEliminacao[MAX_JOGADORES][TAMANHO_NOME];
extern int placarIndex;
void adicionarAoPlacar(const char* nome);
void insertionSortPlacar(void);
void resetarPlacar(void);

#endif 