#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h> // Para calcular a posição em círculo

// --- Configurações do Jogo ---
#define LARGURA_TELA 800
#define ALTURA_TELA 600

// --- AJUSTE: Número de jogadores agora é variável ---
#define MIN_JOGADORES 2
#define MAX_JOGADORES 12
#define DEFAULT_JOGADORES 6

#define RAIO_CIRCULO 200
#define TAMANHO_FONTE 20
#define TAMANHO_NOME 20

// --- AJUSTE: Caminhos das Imagens ---
// (Ajuste a extensão .png se for outra)
#define PATH_MENU "Sprites/Imagens/bq3.png"
#define PATH_B2 "Sprites/Imagens/bq2.png"
#define PATH_B4 "Sprites/Imagens/bq4.png"
#define PATH_B9 "Sprites/Imagens/bq9.png"
#define PATH_B6 "Sprites/Imagens/bq6.png"


// Enum para os estados do jogo (Menu)
typedef enum GameScreen {
    MENU,
    GAMEPLAY,
    END_GAME
} GameScreen;

// --- Estrutura de Dados (Conforme seu exemplo) ---
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

ListaCircular* criarLista() {
    ListaCircular* lista = (ListaCircular*)malloc(sizeof(ListaCircular));
    if (lista != NULL) {
        lista->head = NULL;
        lista->tail = NULL;
        lista->tamanho = 0;
    }
    return lista;
}

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

int getTamanho(ListaCircular* lista) {
    return lista->tamanho;
}

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

// --- AJUSTE: Array de placar agora usa MAX_JOGADORES ---
char placarEliminacao[MAX_JOGADORES][TAMANHO_NOME];
int placarIndex = 0;

void adicionarAoPlacar(const char* nome) {
    // --- AJUSTE: Checa contra MAX_JOGADORES ---
    if (placarIndex < MAX_JOGADORES) {
        strcpy(placarEliminacao[placarIndex], nome);
        placarIndex++;
    }
}

// Selection Sort (Ordenação por Seleção)
void ordenarPlacar() {
    for (int i = 0; i < placarIndex - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < placarIndex; j++) {
            if (strcmp(placarEliminacao[j], placarEliminacao[min_idx]) < 0) {
                min_idx = j;
            }
        }
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

    // --- AJUSTE: Carregando as texturas (Imagens) ---
    Texture2D texMenu = LoadTexture(PATH_MENU);
    Texture2D texBatataPassando1 = LoadTexture(PATH_B2);
    Texture2D texBatataPassando2 = LoadTexture(PATH_B4);
    Texture2D texBatataPassando3 = LoadTexture(PATH_B9);
    Texture2D texBatataQueimou = LoadTexture(PATH_B6);

    // Array para animação
    Texture2D texAnimBatata[3] = { texBatataPassando1, texBatataPassando2, texBatataPassando3 };
    int frameAnimBatata = 0; // Frame atual da animação

    GameScreen telaAtual = MENU; // Começa no Menu
    
    // --- AJUSTE: Variáveis do Menu ---
    int menuSelecao = 0; // 0: Iniciar, 1: Jogadores, 2: Sair
    int numJogadoresAtual = DEFAULT_JOGADORES;
    bool querSair = false; // Para fechar o jogo
    
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
    // --- AJUSTE: Adicionado !querSair ---
    while (!WindowShouldClose() && !querSair) {

        // --- Lógica de Atualização (Update) ---
        switch (telaAtual) {
            case MENU: {
                // Navegação no Menu
                if (IsKeyPressed(KEY_DOWN)) {
                    menuSelecao = (menuSelecao + 1) % 3; // 0, 1, 2
                }
                if (IsKeyPressed(KEY_UP)) {
                    menuSelecao = (menuSelecao - 1 + 3) % 3; // 0, 1, 2
                }

                // Interação com o Menu
                if (menuSelecao == 1) { // Opção "Jogadores"
                    if (IsKeyPressed(KEY_RIGHT) && numJogadoresAtual < MAX_JOGADORES) {
                        numJogadoresAtual++;
                    }
                    if (IsKeyPressed(KEY_LEFT) && numJogadoresAtual > MIN_JOGADORES) {
                        numJogadoresAtual--;
                    }
                } else if (IsKeyPressed(KEY_ENTER)) {
                    switch (menuSelecao) {
                        case 0: { // INICIAR
                            // Limpa estado anterior (se houver)
                            while (getTamanho(listaJogadores) > 0) {
                                removerJogador(listaJogadores, listaJogadores->head);
                            }
                            placarIndex = 0;
                            placarFoiOrdenado = false;
                            frameAnimBatata = 0;

                            // [LÓGICA DS] Cria o círculo de jogadores (com o número escolhido)
                            for (int i = 0; i < numJogadoresAtual; i++) {
                                float angulo = (float)i / numJogadoresAtual * (2 * PI); // Posição em radianos
                                Vector2 pos = {
                                    centroTela.x + RAIO_CIRCULO * cosf(angulo),
                                    centroTela.y + RAIO_CIRCULO * sinf(angulo)
                                };
                                char nome[TAMANHO_NOME];
                                sprintf(nome, "Jogador %d", i + 1);
                                Color cor = { (unsigned char)GetRandomValue(100, 250),
                                              (unsigned char)GetRandomValue(100, 250),
                                              (unsigned char)GetRandomValue(100, 250), 255 };
                                
                                inserirJogador(listaJogadores, nome, pos, cor);
                            }
                            
                            batataAtual = listaJogadores->head; // Começa com o primeiro
                            timerMusica = (float)GetRandomValue(3, 8); // "Música" toca por 3-8 seg
                            telaAtual = GAMEPLAY;
                        } break;
                        
                        case 1: // Opção "Jogadores" - Enter não faz nada
                            break; 
                            
                        case 2: { // SAIR
                            querSair = true;
                        } break;
                    }
                }
            } break;

            case GAMEPLAY: {
                if (timerQueimou > 0.0f) { // Se alguém queimou, pausa o jogo
                    timerQueimou -= GetFrameTime();
                    
                    if (timerQueimou <= 0.0f) { // Fim da pausa, remove o jogador
                        
                        Jogador* jogadorEliminado = batataAtual;
                        
                        adicionarAoPlacar(jogadorEliminado->nome);
                        
                        batataAtual = batataAtual->prox; 

                        removerJogador(listaJogadores, jogadorEliminado);
                        imprimirListaConsole(listaJogadores); // Debug

                        if (getTamanho(listaJogadores) == 1) {
                            adicionarAoPlacar(listaJogadores->head->nome);
                            telaAtual = END_GAME;
                        } else {
                            timerMusica = (float)GetRandomValue(3, 8);
                        }
                    }
                } else { // Jogo rodando normal
                    timerMusica -= GetFrameTime();
                    timerPasso -= GetFrameTime();

                    if (timerMusica <= 0.0f) {
                        timerQueimou = 2.0f; // Pausa por 2 segundos
                    }
                    else if (timerPasso <= 0.0f) {
                        // [LÓGICA DS] Esta é a navegação na estrutura!
                        batataAtual = batataAtual->prox;
                        
                        // --- AJUSTE: Atualiza o frame da animação ---
                        frameAnimBatata = (frameAnimBatata + 1) % 3; // Cicla 0, 1, 2
                        
                        timerPasso = 0.2f; // Reseta velocidade
                    }
                }
            } break;

            case END_GAME: {
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
                // --- AJUSTE: Desenha a imagem do menu ---
                // Centraliza a imagem
                float imgX = centroTela.x - texMenu.width / 2.0f;
                float imgY = 100;
                DrawTexture(texMenu, imgX, imgY, WHITE);
                
                // Desenha as opções do menu
                int posY = 350;
                DrawText("Iniciar", centroTela.x - MeasureText("Iniciar", 30) / 2, posY, 30, (menuSelecao == 0) ? MAROON : DARKGRAY);
                posY += 40;
                DrawText(TextFormat("Jogadores: < %d >", numJogadoresAtual), centroTela.x - MeasureText(TextFormat("Jogadores: < %d >", numJogadoresAtual), 30) / 2, posY, 30, (menuSelecao == 1) ? MAROON : DARKGRAY);
                posY += 40;
                DrawText("Sair", centroTela.x - MeasureText("Sair", 30) / 2, posY, 30, (menuSelecao == 2) ? MAROON : DARKGRAY);
                
                DrawText("Use SETAS (CIMA/BAIXO) para navegar", 10, ALTURA_TELA - 30, 20, GRAY);
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

                // --- AJUSTE: Desenha a batata com Texturas ---
                if (batataAtual != NULL) {
                    Texture2D texBatataAtual;
                    
                    if (timerQueimou > 0) {
                        // Se queimou, usa a bq6
                        texBatataAtual = texBatataQueimou;
                    } else {
                        // Senão, usa o frame atual da animação (bq2, bq4, bq9)
                        texBatataAtual = texAnimBatata[frameAnimBatata];
                    }

                    // Centraliza a textura no jogador
                    Vector2 texPos = {
                        batataAtual->posTela.x - texBatataAtual.width / 2.0f,
                        batataAtual->posTela.y - texBatataAtual.height / 2.0f
                    };
                    DrawTextureV(texBatataAtual, texPos, WHITE);
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
    // --- AJUSTE: Descarrega todas as texturas ---
    UnloadTexture(texMenu);
    UnloadTexture(texBatataPassando1);
    UnloadTexture(texBatataPassando2);
    UnloadTexture(texBatataPassando3);
    UnloadTexture(texBatataQueimou);
    
    // Limpa a lista
    while (getTamanho(listaJogadores) > 0) {
        removerJogador(listaJogadores, listaJogadores->head);
    }
    free(listaJogadores);
    CloseWindow();

    return 0;
}