#include "jogador.h"
#include <stdlib.h> 
#include <string.h> 
#include <stdio.h>


#define ESCALA_JOGADOR 0.045f
extern Texture2D texturasJogadores[MAX_JOGADORES];

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

Jogador* criarJogador(const char* nome, Vector2 pos, Color cor, bool ehHumano, int indiceSprite) {
    Jogador* novo = (Jogador*)malloc(sizeof(Jogador));
    if (novo == NULL) return NULL;
    
    strncpy(novo->nome, nome, TAMANHO_NOME - 1);
    novo->nome[TAMANHO_NOME - 1] = '\0';
    novo->posTela = pos;
    novo->cor = cor;
    novo->ehHumano = ehHumano;
    novo->indiceSprite = indiceSprite; 
    novo->pontuacao = 0.0f;
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

Jogador* passarBatata(Jogador* jogadorAtual, int passos) {
    if (jogadorAtual == NULL) return NULL;
    
    Jogador* temp = jogadorAtual;
    for (int i = 0; i < passos; i++) {
        if (temp != NULL) {
            temp = temp->prox;
        }
    }
    return temp;
}

void desenharJogadorNaTela(Jogador* j) {
    if (j == NULL) return;
    Texture2D sprite = texturasJogadores[j->indiceSprite];
    float escala = ESCALA_JOGADOR; 
    Vector2 origin = {
        (sprite.width * escala) / 2.0f,
        (sprite.height * escala) / 2.0f
    };
    Rectangle sourceRect = { 0.0f, 0.0f, (float)sprite.width, (float)sprite.height };
    Rectangle destRect = {
        j->posTela.x,
        j->posTela.y,
        sprite.width * escala,
        sprite.height * escala
    };
    DrawTexturePro(sprite, sourceRect, destRect, origin, 0.0f, WHITE);
    int fontSize = 15;
    float textWidth = MeasureText(j->nome, fontSize);
    float nomePosY = j->posTela.y + origin.y + 10; 
    
    DrawText(j->nome, j->posTela.x - textWidth / 2, nomePosY, fontSize, BLACK);

    char pontuacaoTexto[20];
    sprintf(pontuacaoTexto, "%.0fpts", j->pontuacao);
    textWidth = MeasureText(pontuacaoTexto, 10);
    DrawText(pontuacaoTexto, j->posTela.x - textWidth / 2, nomePosY + 17, 10, DARKGRAY);
}