#ifndef JOGADOR_H
#define JOGADOR_H

#include "raylib.h"
#include "utils.h" 

typedef struct Jogador {
    char nome[TAMANHO_NOME];
    Vector2 posTela;
    Color cor;
    bool ehHumano;
    int indiceSprite;
    float pontuacao; 
    struct Jogador* prox;
} Jogador;

typedef struct {
    Jogador* head;
    Jogador* tail;
    int tamanho;
} ListaCircular;

extern Texture2D texturasJogadores[MAX_JOGADORES];

ListaCircular* criarRoda(void);
void destruirRoda(ListaCircular* roda);
Jogador* criarJogador(const char* nome, Vector2 pos, Color cor, bool ehHumano, int indiceSprite);
void inserirNaRoda(ListaCircular* roda, Jogador* novoJogador);
Jogador* removerDaRoda(ListaCircular* roda, Jogador* jogadorEliminado);
int contarJogadores(ListaCircular* roda);
Jogador* passarBatata(Jogador* jogadorAtual, int passos);
void desenharJogadorNaTela(Jogador* j);

#endif