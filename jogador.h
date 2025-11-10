#ifndef JOGADOR_H
#define JOGADOR_H

#include "utils.h" 

typedef struct Jogador {
    char nome[TAMANHO_NOME];
    Vector2 posTela;
    Color cor;
    bool ehHumano;
    struct Jogador* prox;
} Jogador;

typedef struct {
    Jogador* head;
    Jogador* tail;
    int tamanho;
} ListaCircular;



ListaCircular* criarRoda(void);
void destruirRoda(ListaCircular* roda);
Jogador* criarJogador(const char* nome, Vector2 pos, Color cor, bool ehHumano);
void inserirNaRoda(ListaCircular* roda, Jogador* novoJogador);
Jogador* removerDaRoda(ListaCircular* roda, Jogador* jogadorEliminado);
int contarJogadores(ListaCircular* roda);
Jogador* passarBatata(Jogador* jogadorAtual);
void desenharJogadorNaTela(Jogador* j);

#endif 