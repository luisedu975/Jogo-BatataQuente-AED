#include "jogador.h"
#include <stdlib.h> 
#include <string.h> 

ListaCircular* criarRoda(void) {
    ListaCircular* lista = (ListaCircular*)malloc(sizeof(ListaCircular));
    if (lista != NULL) {
        lista->head = NULL;
        lista->tail = NULL;
        lista->tamanho = 0;
    }
    return lista;
}

void destruirRoda(ListaCircular* roda) {
    if (roda == NULL) return;
    while (contarJogadores(roda) > 0) {
        removerDaRoda(roda, roda->head);
    }
    free(roda);
}

Jogador* criarJogador(const char* nome, Vector2 pos, Color cor, bool ehHumano) {
    Jogador* novo = (Jogador*)malloc(sizeof(Jogador));
    if (novo == NULL) return NULL;
    
    strncpy(novo->nome, nome, TAMANHO_NOME - 1);
    novo->nome[TAMANHO_NOME - 1] = '\0';
    novo->posTela = pos;
    novo->cor = cor;
    novo->ehHumano = ehHumano;
    novo->prox = NULL; 
    
    return novo;
}

void inserirNaRoda(ListaCircular* roda, Jogador* novoJogador) {
    if (roda == NULL || novoJogador == NULL) return;

    if (roda->head == NULL) {
        roda->head = novoJogador;
        roda->tail = novoJogador;
        novoJogador->prox = novoJogador; 
    } else {
        novoJogador->prox = roda->head;
        roda->head = novoJogador;
        roda->tail->prox = roda->head; 
    }
    roda->tamanho++;
}

Jogador* removerDaRoda(ListaCircular* roda, Jogador* jogadorEliminado) {
    if (roda->head == NULL || jogadorEliminado == NULL) return NULL;

    Jogador* anterior = roda->tail;
    while (anterior->prox != jogadorEliminado) {
        anterior = anterior->prox;
        if (anterior == roda->tail) return roda->head; 
    }

    Jogador* proximoJogador = jogadorEliminado->prox;

    if (roda->tamanho == 1) {
        roda->head = NULL;
        roda->tail = NULL;
        proximoJogador = NULL; 
    } else {
        if (jogadorEliminado == roda->head) {
            roda->head = jogadorEliminado->prox;
        }
        if (jogadorEliminado == roda->tail) {
            roda->tail = anterior;
        }
        anterior->prox = jogadorEliminado->prox;
    }
    
    free(jogadorEliminado);
    roda->tamanho--;
    
    return proximoJogador; 
}

int contarJogadores(ListaCircular* roda) {
    if (roda == NULL) return 0;
    return roda->tamanho;
}

Jogador* passarBatata(Jogador* jogadorAtual) {
    if (jogadorAtual == NULL) return NULL;
    return jogadorAtual->prox;
}

void desenharJogadorNaTela(Jogador* j) {
    Vector2 pos = j->posTela;
    DrawCircleV((Vector2){pos.x, pos.y - 35}, 15, j->cor);
    DrawRectangleV((Vector2){pos.x - 12, pos.y - 20}, (Vector2){24, 40}, j->cor);
    DrawLine(pos.x - 28, pos.y, pos.x + 28, pos.y, j->cor);
    DrawLine(pos.x - 12, pos.y + 20, pos.x - 6, pos.y + 50, j->cor);
    DrawLine(pos.x + 12, pos.y + 20, pos.x + 6, pos.y + 50, j->cor);
    
    int fontSize = 15;
    float textWidth = MeasureText(j->nome, fontSize);
    DrawText(j->nome, pos.x - textWidth / 2, pos.y + 55, fontSize, BLACK);
}