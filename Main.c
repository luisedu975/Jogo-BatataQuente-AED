#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h> // Para calcular a posição em círculo

// --- Configurações do Jogo ---
#define LARGURA_TELA 800
#define ALTURA_TELA 600
#define NUM_JOGADORES 6
#define RAIO_CIRCULO 200
#define TAMANHO_FONTE 20
#define TAMANHO_NOME 20

// Enum para os estados do jogo (Menu) 
typedef enum GameScreen {
    MENU,
    GAMEPLAY,
    END_GAME
} GameScreen;

// --- Estrutura de Dados (Conforme seu exemplo) --- [cite: 18]
typedef struct Jogador {
    char nome[TAMANHO_NOME];
    Vector2 posTela; // Posição para desenhar na raylib
    Color cor;
    struct Jogador* prox;
} Jogador;

// Estrutura para gerenciar a lista
typedef struct {
    Jogador* head;
    Jogador* tail;
    int tamanho;
} ListaCircular;

// --- Funções da Estrutura de Dados (Requisito 3.c / 3.e) --- 

/**
 * [Função DS 1] Cria e inicializa uma nova lista circular vazia.
 */
ListaCircular* criarLista() {
    ListaCircular* lista = (ListaCircular*)malloc(sizeof(ListaCircular));
    if (lista != NULL) {
        lista->head = NULL;
        lista->tail = NULL;
        lista->tamanho = 0;
    }
    return lista;
}

/**
 * [Função DS 2] Insere um novo jogador na lista (baseado no seu código de 'inserir').
 */
void inserirJogador(ListaCircular* lista, const char* nome, Vector2 pos, Color cor) {
    Jogador* novo = (Jogador*)malloc(sizeof(Jogador));
    if (novo == NULL) return;

    strcpy(novo->nome, nome);
    novo->posTela = pos;
    novo->cor = cor;

    if (lista->head == NULL) { // Lista vazia
        lista->head = novo;
        lista->tail = novo;
    } else { // Insere no 'head' (como seu exemplo)
        novo->prox = lista->head;
        lista->head = novo;
    }
    // Mantém a circularidade
    lista->tail->prox = lista->head;
    lista->tamanho++;
}

/**
 * [Função DS 3] Remove um jogador específico da lista.
 * Esta é a lógica central $O(n)$ que discutimos.
 */
void removerJogador(ListaCircular* lista, Jogador* jogadorEliminado) {
    if (lista->head == NULL) return; // Lista vazia

    Jogador* anterior = lista->tail; // Começa pelo 'tail' para achar o anterior
    
    // 1. Acha o jogador ANTERIOR ao que será eliminado
    while (anterior->prox != jogadorEliminado) {
        anterior = anterior->prox;
    }

    // 2. Trata os casos de remoção
    if (lista->tamanho == 1) { // Removendo o último
        lista->head = NULL;
        lista->tail = NULL;
    } else {
        // 3. Re-liga os ponteiros
        if (jogadorEliminado == lista->head) {
            lista->head = jogadorEliminado->prox;
        }
        if (jogadorEliminado == lista->tail) {
            lista->tail = anterior;
        }
        anterior->prox = jogadorEliminado->prox; // Pula o eliminado
    }

    // 4. Libera a memória e atualiza o tamanho
    free(jogadorEliminado);
    lista->tamanho--;
}

/**
 * [Função DS 4] Retorna o tamanho atual da lista.
 */
int getTamanho(ListaCircular* lista) {
    return lista->tamanho;
}

/**
 * [Função DS 5] Imprime a lista no console (para debug).
 */
void imprimirListaConsole(ListaCircular* lista) {
    if (lista->head == NULL) {
        printf("LISTA VAZIA\n");
        return;
    }
    Jogador* temp = lista->head;
    printf("Lista (Tamanho: %d): ", lista->tamanho);
    do {
        printf("%s -> ", temp->nome);
        temp = temp->prox;
    } while (temp != lista->head);
    printf("... (volta ao %s)\n", lista->head->nome);
}

// --- Funções do Algoritmo de Ordenação (Requisito 3.d) --- 

// Armazena a ordem de eliminação
char placarEliminacao[NUM_JOGADORES][TAMANHO_NOME];
int placarIndex = 0;

void adicionarAoPlacar(const char* nome) {
    if (placarIndex < NUM_JOGADORES) {
        strcpy(placarEliminacao[placarIndex], nome);
        placarIndex++;
    }
}

// Selection Sort (Ordenação por Seleção)
void ordenarPlacar() {
    for (int i = 0; i < placarIndex - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < placarIndex; j++) {
            // Compara strings
            if (strcmp(placarEliminacao[j], placarEliminacao[min_idx]) < 0) {
                min_idx = j;
            }
        }
        // Troca (swap)
        char temp[TAMANHO_NOME];
        strcpy(temp, placarEliminacao[min_idx]);
        strcpy(placarEliminacao[min_idx], placarEliminacao[i]);
        strcpy(placarEliminacao[i], temp);
    }
}

// --- Função Principal (main) com Raylib ---

int main(void) {
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Batata Quente Circular (AED)");
    SetTargetFPS(60);
    srand(time(NULL));

    GameScreen telaAtual = MENU; // Começa no Menu 
    
    // Variáveis de estado do jogo
    ListaCircular* listaJogadores = criarLista();
    Jogador* batataAtual = NULL;
    
    float timerMusica = 0.0f;     // Tempo da rodada
    float timerPasso = 0.2f;      // Velocidade que a batata passa
    float timerQueimou = 0.0f;  // Timer para mostrar "QUEIMOU!"
    
    bool placarFoiOrdenado = false;

    // Centro da tela
    Vector2 centroTela = { LARGURA_TELA / 2.0f, ALTURA_TELA / 2.0f };

    // --- Loop Principal do Jogo ---
    while (!WindowShouldClose()) {

        // --- Lógica de Atualização (Update) ---
        switch (telaAtual) {
            case MENU: {
                if (IsKeyPressed(KEY_ENTER)) {
                    // Limpa estado anterior (se houver)
                    while (getTamanho(listaJogadores) > 0) {
                        removerJogador(listaJogadores, listaJogadores->head);
                    }
                    placarIndex = 0;
                    placarFoiOrdenado = false;

                    // [LÓGICA DS] Cria o círculo de jogadores
                    for (int i = 0; i < NUM_JOGADORES; i++) {
                        float angulo = (float)i / NUM_JOGADORES * (2 * PI); // Posição em radianos
                        Vector2 pos = {
                            centroTela.x + RAIO_CIRCULO * cosf(angulo),
                            centroTela.y + RAIO_CIRCULO * sinf(angulo)
                        };
                        char nome[TAMANHO_NOME];
                        sprintf(nome, "Jogador %d", i + 1);
                        // Cor aleatória
                        Color cor = { (unsigned char)GetRandomValue(100, 250),
                                      (unsigned char)GetRandomValue(100, 250),
                                      (unsigned char)GetRandomValue(100, 250), 255 };
                        
                        // [CHAMADA DS] Inserindo na lista
                        inserirJogador(listaJogadores, nome, pos, cor);
                    }
                    
                    batataAtual = listaJogadores->head; // Começa com o primeiro
                    timerMusica = (float)GetRandomValue(3, 8); // "Música" toca por 3-8 seg
                    telaAtual = GAMEPLAY;
                }
            } break;

            case GAMEPLAY: {
                if (timerQueimou > 0.0f) { // Se alguém queimou, pausa o jogo
                    timerQueimou -= GetFrameTime();
                    
                    if (timerQueimou <= 0.0f) { // Fim da pausa, remove o jogador
                        
                        Jogador* jogadorEliminado = batataAtual;
                        
                        // Salva para o placar 
                        adicionarAoPlacar(jogadorEliminado->nome);
                        
                        // [LÓGICA DS] A batata passa para o próximo ANTES de remover
                        batataAtual = batataAtual->prox; 

                        // [CHAMADA DS] Remove o jogador da lista
                        removerJogador(listaJogadores, jogadorEliminado);
                        imprimirListaConsole(listaJogadores); // Debug

                        // Verifica condição de vitória
                        if (getTamanho(listaJogadores) == 1) {
                            adicionarAoPlacar(listaJogadores->head->nome); // Adiciona o vencedor
                            telaAtual = END_GAME;
                        } else {
                            // Prepara nova rodada
                            timerMusica = (float)GetRandomValue(3, 8);
                        }
                    }
                } else { // Jogo rodando normal
                    timerMusica -= GetFrameTime();
                    timerPasso -= GetFrameTime();

                    // "Música" parou (Queimou!)
                    if (timerMusica <= 0.0f) {
                        timerQueimou = 2.0f; // Pausa por 2 segundos
                    }
                    // Passa a batata
                    else if (timerPasso <= 0.0f) {
                        // [LÓGICA DS] Esta é a navegação na estrutura! 
                        batataAtual = batataAtual->prox;
                        timerPasso = 0.2f; // Reseta velocidade
                    }
                }
            } break;

            case END_GAME: {
                // [CHAMADA DS/ORDENAÇÃO] Ordena o placar, mas só uma vez 
                if (!placarFoiOrdenado) {
                    //ordenarPlacar(); // Ative se quiser ordenar por nome
                    placarFoiOrdenado = true;
                }

                if (IsKeyPressed(KEY_ENTER)) {
                    telaAtual = MENU;
                }
            } break;
        }

        // --- Lógica de Desenho (Draw) ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (telaAtual) {
            case MENU: {
                DrawText("BATATA QUENTE CIRCULAR", centroTela.x - MeasureText("BATATA QUENTE CIRCULAR", 40) / 2, centroTela.y - 40, 40, DARKGRAY);
                DrawText("Pressione ENTER para começar", centroTela.x - MeasureText("Pressione ENTER para começar", 20) / 2, centroTela.y + 20, 20, GRAY);
            } break;

            case GAMEPLAY: {
                // Desenha todos os jogadores na lista
                if (listaJogadores->head != NULL) {
                    Jogador* temp = listaJogadores->head;
                    do {
                        DrawCircleV(temp->posTela, 20, temp->cor);
                        DrawText(temp->nome, temp->posTela.x - 30, temp->posTela.y + 25, 15, BLACK);
                        temp = temp->prox;
                    } while (temp != listaJogadores->head);
                }

                // Desenha a batata
                if (batataAtual != NULL) {
                    DrawCircleV(batataAtual->posTela, 25, (timerQueimou > 0) ? ORANGE : RED);
                }

                // Desenha Timer
                DrawText(TextFormat("Tempo: %.1f s", (timerMusica > 0) ? timerMusica : 0.0f), 10, 10, TAMANHO_FONTE, DARKGRAY);

                if (timerQueimou > 0.0f) {
                    DrawText("QUEIMOU!", centroTela.x - MeasureText("QUEIMOU!", 60) / 2, centroTela.y - 30, 60, MAROON);
                    DrawText(batataAtual->nome, centroTela.x - MeasureText(batataAtual->nome, 30) / 2, centroTela.y + 40, 30, MAROON);
                }
            } break;

            case END_GAME: {
                // O vencedor é o único que sobrou na lista
                const char* vencedor = listaJogadores->head->nome;
                DrawText("FIM DE JOGO!", centroTela.x - MeasureText("FIM DE JOGO!", 40) / 2, 50, 40, DARKGRAY);
                DrawText(TextFormat("O VENCEDOR É: %s", vencedor), centroTela.x - MeasureText(TextFormat("O VENCEDOR É: %s", vencedor), 30) / 2, 100, 30, GOLD);

                // Desenha o placar de eliminação 
                DrawText("Ordem de Eliminacao (do ultimo pro primeiro):", 150, 180, 20, DARKGRAY);
                for (int i = 0; i < placarIndex - 1; i++) { // -1 para não mostrar o vencedor
                     DrawText(TextFormat("%d. %s", (placarIndex - 1 - i), placarEliminacao[i]), 200, 220 + (i * 30), 20, GRAY);
                }

                DrawText("Pressione ENTER para voltar ao Menu", centroTela.x - MeasureText("Pressione ENTER para voltar ao Menu", 20) / 2, 550, 20, GRAY);
            } break;
        }

        EndDrawing();
    }

    // --- Limpeza ---
    while (getTamanho(listaJogadores) > 0) {
        removerJogador(listaJogadores, listaJogadores->head);
    }
    free(listaJogadores);
    CloseWindow();

    return 0;
}