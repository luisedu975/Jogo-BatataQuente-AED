#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h> // Para calcular a posição em círculo

// --- Configurações do Jogo ---
#define LARGURA_TELA 800
#define ALTURA_TELA 600

#define MIN_JOGADORES 2
#define MAX_JOGADORES 12
#define DEFAULT_JOGADORES 6

#define RAIO_CIRCULO 200
#define TAMANHO_FONTE 20
#define TAMANHO_NOME 20 // Limite de caracteres para o nome

#define ESCALA_BATATA 0.25f
#define ESCALA_MENU 0.5f 

// Caminhos das Imagens
#define PATH_MENU "Sprites/Imagens/menu principal.png"
#define PATH_B2 "Sprites/Imagens/bq2.png"
#define PATH_B4 "Sprites/Imagens/bq4.png"
#define PATH_B9 "Sprites/Imagens/bq9.png"
#define PATH_B6 "Sprites/Imagens/bq6.png"
#define PATH_B5 "Sprites/Imagens/bq5.png"


// Enum para os estados do jogo
typedef enum GameScreen {
    MENU,
    CUSTOMIZE_NAMES,
    GAMEPLAY,
    END_GAME
} GameScreen;

// --- AJUSTE: O enum agora tem todas as 4 opções ---
typedef enum {
    ALEATORIO,     // 0
    CRESCENTE,     // 1
    DECRESCENTE,   // 2
    PERSONALIZADO  // 3
} ModoTimer;

// --- Estrutura de Dados ---
// (Oculta para encurtar, sem alterações)
typedef struct Jogador {
    char nome[TAMANHO_NOME];
    Vector2 posTela;
    Color cor;
    struct Jogador* prox;
} Jogador;

typedef struct {
    Jogador* head;
    Jogador* tail;
    int tamanho;
} ListaCircular;

// --- Funções da Estrutura de Dados ---
// (Ocultas para encurtar, sem alterações)
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
    strncpy(novo->nome, nome, TAMANHO_NOME - 1);
    novo->nome[TAMANHO_NOME - 1] = '\0';
    novo->posTela = pos;
    novo->cor = cor;
    if (lista->head == NULL) {
        lista->head = novo;
        lista->tail = novo;
    } else {
        novo->prox = lista->head;
        lista->head = novo;
    }
    lista->tail->prox = lista->head;
    lista->tamanho++;
}

void removerJogador(ListaCircular* lista, Jogador* jogadorEliminado) {
    if (lista->head == NULL) return;
    Jogador* anterior = lista->tail;
    while (anterior->prox != jogadorEliminado) {
        anterior = anterior->prox;
    }
    if (lista->tamanho == 1) {
        lista->head = NULL;
        lista->tail = NULL;
    } else {
        if (jogadorEliminado == lista->head) {
            lista->head = jogadorEliminado->prox;
        }
        if (jogadorEliminado == lista->tail) {
            lista->tail = anterior;
        }
        anterior->prox = jogadorEliminado->prox;
    }
    free(jogadorEliminado);
    lista->tamanho--;
}

int getTamanho(ListaCircular* lista) {
    return lista->tamanho;
}

void imprimirListaConsole(ListaCircular* lista) {
    // (Oculta para encurtar)
}


// --- Funções do Algoritmo de Ordenação ---
// (Ocultas para encurtar, sem alterações)
char placarEliminacao[MAX_JOGADORES][TAMANHO_NOME];
int placarIndex = 0;

void adicionarAoPlacar(const char* nome) {
    if (placarIndex < MAX_JOGADORES) {
        strcpy(placarEliminacao[placarIndex], nome);
        placarIndex++;
    }
}

void ordenarPlacar() {
    // (Oculta para encurtar)
}


// --- Funções de Lógica de Jogo (Modo Timer) ---

/**
 * Converte o enum ModoTimer para texto
 */
const char* getModoTimerTexto(ModoTimer modo) {
    switch (modo) {
        // --- AJUSTE: Adiciona os novos textos ---
        case CRESCENTE: return "Crescente";
        case DECRESCENTE: return "Decrescente";
        case PERSONALIZADO: return "Personalizado";
        case ALEATORIO: default: return "Aleatorio";
    }
}

/**
 * Calcula o próximo timer da "música" baseado no modo de jogo.
 */
float getNovoTimer(ModoTimer modo, float tempoPersonalizado, int numJogadoresInicio, int numEliminados) {
    // --- AJUSTE: Lógica completa re-implementada ---

    // Caso 1: Personalizado (tempo fixo)
    if (modo == PERSONALIZADO) {
        return tempoPersonalizado;
    }

    float minTime, maxTime;
    int totalRodadas = numJogadoresInicio - 1; // Total de eliminações no jogo
    int rodadaAtual = numEliminados; // Quantos já saíram

    // Evita divisão por zero se houver 2 jogadores (1 rodada total)
    if (totalRodadas <= 0) totalRodadas = 1; 

    switch (modo) {
        case CRESCENTE:
            // Começa rápido (1.5-2.5s) e fica lento (7-9s)
            minTime = 1.5f + (5.5f * ((float)rodadaAtual / totalRodadas));
            maxTime = 2.5f + (6.5f * ((float)rodadaAtual / totalRodadas));
            break;
        case DECRESCENTE:
            // Começa lento (7-9s) e fica rápido (1.5-2.5s)
            minTime = 7.0f - (5.5f * ((float)rodadaAtual / totalRodadas));
            maxTime = 9.0f - (6.5f * ((float)rodadaAtual / totalRodadas));
            break;
        case ALEATORIO:
        default:
            minTime = 3.0f;
            maxTime = 8.0f;
            break;
    }
    
    // Garante que os tempos mínimos não sejam muito baixos
    if (minTime < 1.0f) minTime = 1.0f;
    if (maxTime <= minTime) maxTime = minTime + 1.0f;

    // Retorna um valor aleatório entre minTime e maxTime
    return (float)GetRandomValue((int)(minTime * 100), (int)(maxTime * 100)) / 100.0f;
}


// --- Função Principal (main) com Raylib ---

int main(void) {
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Batata Quente Circular (AED)");
    SetTargetFPS(60);
    srand(time(NULL));

    // Carregando as texturas
    Texture2D texMenu = LoadTexture(PATH_MENU);
    Texture2D texCustomize = LoadTexture(PATH_B5);
    Texture2D texBatataPassando1 = LoadTexture(PATH_B2);
    Texture2D texBatataPassando2 = LoadTexture(PATH_B4);
    Texture2D texBatataPassando3 = LoadTexture(PATH_B9);
    Texture2D texBatataQueimou = LoadTexture(PATH_B6);

    Texture2D texAnimBatata[3] = { texBatataPassando1, texBatataPassando2, texBatataPassando3 };
    int frameAnimBatata = 0;

    GameScreen telaAtual = MENU;
    
    // Variáveis do Menu
    int menuSelecao = 0;
    int numJogadoresAtual = DEFAULT_JOGADORES;
    ModoTimer modoTimerAtual = ALEATORIO;
    float tempoPersonalizado = 5.0f;
    int totalMenuOpcoes = 4;
    bool querSair = false;

    // Variáveis da Tela de Personalização
    char playerNames[MAX_JOGADORES][TAMANHO_NOME];
    int nameBoxSelecao = 0;
    bool nameBoxAtiva = false;
    int nameCharCount = 0;
    
    // Variáveis de estado do jogo
    ListaCircular* listaJogadores = criarLista();
    Jogador* batataAtual = NULL;
    
    float timerMusica = 0.0f;
    float timerPasso = 0.2f;
    float timerQueimou = 0.0f;
    
    bool placarFoiOrdenado = false;

    Vector2 centroTela = { LARGURA_TELA / 2.0f, ALTURA_TELA / 2.0f };

    // --- Loop Principal do Jogo ---
    while (!WindowShouldClose() && !querSair) {

        // --- Lógica de Atualização (Update) ---
        switch (telaAtual) {
            case MENU: {
                totalMenuOpcoes = (modoTimerAtual == PERSONALIZADO) ? 5 : 4;

                if (IsKeyPressed(KEY_DOWN)) {
                    menuSelecao = (menuSelecao + 1) % totalMenuOpcoes;
                }
                if (IsKeyPressed(KEY_UP)) {
                    menuSelecao = (menuSelecao - 1 + totalMenuOpcoes) % totalMenuOpcoes;
                }

                if (menuSelecao == 1) { // Jogadores
                    if (IsKeyPressed(KEY_RIGHT) && numJogadoresAtual < MAX_JOGADORES) {
                        numJogadoresAtual++;
                    }
                    if (IsKeyPressed(KEY_LEFT) && numJogadoresAtual > MIN_JOGADORES) {
                        numJogadoresAtual--;
                    }
                } 
                else if (menuSelecao == 2) { // Modo Timer
                    // --- AJUSTE: Agora cicla entre 4 opções ---
                    if (IsKeyPressed(KEY_RIGHT)) {
                        modoTimerAtual = (modoTimerAtual + 1) % 4; // 0, 1, 2, 3
                    }
                    if (IsKeyPressed(KEY_LEFT)) {
                        modoTimerAtual = (modoTimerAtual - 1 + 4) % 4;
                    }
                } 
                else if (menuSelecao == 3 && modoTimerAtual == PERSONALIZADO) { // Tempo Fixo
                    if (IsKeyPressed(KEY_RIGHT) && tempoPersonalizado < 20.0f) {
                        tempoPersonalizado += 0.5f;
                    }
                    if (IsKeyPressed(KEY_LEFT) && tempoPersonalizado > 1.0f) {
                        tempoPersonalizado -= 0.5f;
                    }
                }
                else if (IsKeyPressed(KEY_ENTER)) {
                    int acaoSair = (modoTimerAtual == PERSONALIZADO) ? 4 : 3;

                    if (menuSelecao == 0) { // INICIAR
                        for (int i = 0; i < numJogadoresAtual; i++) {
                            sprintf(playerNames[i], "Jogador %d", i + 1);
                        }
                        nameBoxSelecao = 0;
                        nameBoxAtiva = false;
                        nameCharCount = 0;
                        telaAtual = CUSTOMIZE_NAMES;
                    }
                    else if (menuSelecao == acaoSair) { // SAIR
                        querSair = true;
                    }
                }
            } break;

            case CUSTOMIZE_NAMES: {
                int totalOpcoesNome = numJogadoresAtual + 1; // Nomes + Botão "Confirmar"

                if (nameBoxAtiva) {
                    // --- MODO DE DIGITAÇÃO (COM CORREÇÃO DE BUG) ---
                    
                    int key = GetCharPressed();
                    while (key > 0) {
                        if ((key >= 32) && (key <= 125) && (nameCharCount < TAMANHO_NOME - 1)) {
                            playerNames[nameBoxSelecao][nameCharCount] = (char)key;
                            nameCharCount++;
                            playerNames[nameBoxSelecao][nameCharCount] = '\0'; // <-- CORREÇÃO
                        }
                        key = GetCharPressed();
                    }

                    if (IsKeyPressedRepeat(KEY_BACKSPACE)) {
                        if (nameCharCount > 0) {
                            nameCharCount--;
                            playerNames[nameBoxSelecao][nameCharCount] = '\0'; // <-- CORREÇÃO
                        }
                    }

                    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_UP)) {
                        nameBoxAtiva = false;
                    }
                } 
                else {
                    // --- MODO DE NAVEGAÇÃO ---
                    if (IsKeyPressed(KEY_DOWN)) {
                        nameBoxSelecao = (nameBoxSelecao + 1) % totalOpcoesNome;
                    }
                    if (IsKeyPressed(KEY_UP)) {
                        nameBoxSelecao = (nameBoxSelecao - 1 + totalOpcoesNome) % totalOpcoesNome;
                    }

                    if (IsKeyPressed(KEY_ENTER)) {
                        if (nameBoxSelecao == numJogadoresAtual) {
                            // --- BOTÃO "CONFIRMAR" ---
                            while (getTamanho(listaJogadores) > 0) {
                                removerJogador(listaJogadores, listaJogadores->head);
                            }
                            placarIndex = 0; // Reseta o contador de eliminações
                            placarFoiOrdenado = false;
                            frameAnimBatata = 0;

                            for (int i = 0; i < numJogadoresAtual; i++) {
                                float angulo = (float)i / numJogadoresAtual * (2 * PI);
                                Vector2 pos = {
                                    centroTela.x + RAIO_CIRCULO * cosf(angulo),
                                    centroTela.y + RAIO_CIRCULO * sinf(angulo)
                                };
                                Color cor = { (unsigned char)GetRandomValue(100, 250),
                                              (unsigned char)GetRandomValue(100, 250),
                                              (unsigned char)GetRandomValue(100, 250), 255 };
                                
                                inserirJogador(listaJogadores, playerNames[i], pos, cor); 
                            }
                            
                            batataAtual = listaJogadores->head;
                            // --- AJUSTE: Chama o timer com os parâmetros corretos ---
                            timerMusica = getNovoTimer(modoTimerAtual, tempoPersonalizado, numJogadoresAtual, placarIndex);
                            
                            telaAtual = GAMEPLAY;
                        
                        } else {
                            // Ativa a caixa de texto
                            nameBoxAtiva = true;
                            nameCharCount = strlen(playerNames[nameBoxSelecao]);
                        }
                    }
                }

                if (IsKeyPressed(KEY_ESCAPE)) {
                    telaAtual = MENU;
                }
            } break;

            case GAMEPLAY: {
                if (timerQueimou > 0.0f) {
                    timerQueimou -= GetFrameTime();
                    
                    if (timerQueimou <= 0.0f) {
                        Jogador* jogadorEliminado = batataAtual;
                        adicionarAoPlacar(jogadorEliminado->nome); // placarIndex é incrementado AQUI
                        
                        batataAtual = batataAtual->prox; 
                        removerJogador(listaJogadores, jogadorEliminado);
                        imprimirListaConsole(listaJogadores);

                        if (getTamanho(listaJogadores) == 1) {
                            adicionarAoPlacar(listaJogadores->head->nome);
                            telaAtual = END_GAME;
                        } else {
                            // --- AJUSTE: Chama o timer com os parâmetros corretos ---
                            // placarIndex já está atualizado
                            timerMusica = getNovoTimer(modoTimerAtual, tempoPersonalizado, numJogadoresAtual, placarIndex);
                        }
                    }
                } else {
                    timerMusica -= GetFrameTime();
                    timerPasso -= GetFrameTime();

                    if (timerMusica <= 0.0f) {
                        timerQueimou = 2.0f;
                    }
                    else if (timerPasso <= 0.0f) {
                        batataAtual = batataAtual->prox;
                        frameAnimBatata = (frameAnimBatata + 1) % 3;
                        timerPasso = 0.2f;
                    }
                }
            } break;

            case END_GAME: {
                if (!placarFoiOrdenado) {
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
                // --- AJUSTE: Centralizando e Escalando a imagem do menu ---
                
                Rectangle sourceRect = { 0, 0, (float)texMenu.width, (float)texMenu.height };
                Rectangle destRect = {
                    centroTela.x,
                    180,
                    texMenu.width * ESCALA_MENU,
                    texMenu.height * ESCALA_MENU
                };
                Vector2 origin = {
                    (texMenu.width * ESCALA_MENU) / 2.0f,
                    (texMenu.height * ESCALA_MENU) / 2.0f
                };

                DrawTexturePro(texMenu, sourceRect, destRect, origin, 0.0f, WHITE);
                
                int posY = destRect.y + origin.y - 20; 
                
                DrawText("Iniciar", centroTela.x - MeasureText("Iniciar", 30) / 2, posY, 30, (menuSelecao == 0) ? MAROON : DARKGRAY);
                posY += 40;
                DrawText(TextFormat("Jogadores: < %d >", numJogadoresAtual), centroTela.x - MeasureText(TextFormat("Jogadores: < %d >", numJogadoresAtual), 30) / 2, posY, 30, (menuSelecao == 1) ? MAROON : DARKGRAY);
                posY += 40;
                DrawText(TextFormat("Modo Timer: < %s >", getModoTimerTexto(modoTimerAtual)), centroTela.x - MeasureText(TextFormat("Modo Timer: < %s >", getModoTimerTexto(modoTimerAtual)), 30) / 2, posY, 30, (menuSelecao == 2) ? MAROON : DARKGRAY);
                posY += 40;
                
                int acaoSair = 3;
                if (modoTimerAtual == PERSONALIZADO) {
                    DrawText(TextFormat("Tempo Fixo: < %.1f s >", tempoPersonalizado), centroTela.x - MeasureText(TextFormat("Tempo Fixo: < %.1f s >", tempoPersonalizado), 30) / 2, posY, 30, (menuSelecao == 3) ? MAROON : DARKGRAY);
                    posY += 40;
                    acaoSair = 4;
                }
                DrawText("Sair", centroTela.x - MeasureText("Sair", 30) / 2, posY, 30, (menuSelecao == acaoSair) ? MAROON : DARKGRAY);
                
                DrawText("Use SETAS (CIMA/BAIXO/LADOS) para ajustar", 10, ALTURA_TELA - 30, 20, GRAY);
            } break;

            case CUSTOMIZE_NAMES: {
                // (Desenho da tela de Nomes, com a bq5 - Oculto para encurtar)
                DrawText("Personalize os Nomes", centroTela.x - MeasureText("Personalize os Nomes", 30) / 2, 30, 30, DARKGRAY);
                
                int textBoxPosX = 100;
                for (int i = 0; i < numJogadoresAtual; i++) {
                    Rectangle textBox = { textBoxPosX, 80.0f + i * 40.0f, 300, 30 };
                    
                    if (nameBoxSelecao == i) {
                        DrawRectangleRec(textBox, LIGHTGRAY);
                        DrawRectangleLinesEx(textBox, 2, nameBoxAtiva ? MAROON : DARKGRAY);
                    } else {
                        DrawRectangleRec(textBox, LIGHTGRAY);
                        DrawRectangleLinesEx(textBox, 1, GRAY);
                    }
                    DrawText(playerNames[i], textBox.x + 5, textBox.y + 7, 20, BLACK);

                    if (nameBoxAtiva && nameBoxSelecao == i && ((int)(GetTime() * 2) % 2 == 0)) {
                        float textWidth = MeasureText(playerNames[i], 20);
                        DrawText("|", textBox.x + 5 + textWidth, textBox.y + 7, 20, MAROON);
                    }
                }
                
                int posYBotao = 80 + numJogadoresAtual * 40;
                DrawText("Confirmar e Iniciar", textBoxPosX + 150 - MeasureText("Confirmar e Iniciar", 20) / 2, posYBotao, 20, (nameBoxSelecao == numJogadoresAtual) ? MAROON : DARKGRAY);
                
                float imgX = LARGURA_TELA - texCustomize.width - 40;
                float imgY = centroTela.y - texCustomize.height / 2.5f;
                DrawTexture(texCustomize, (int)imgX, (int)imgY, WHITE);

                DrawText("Use CIMA/BAIXO, ENTER para editar, BACKSPACE para apagar. ESC para voltar.", 10, ALTURA_TELA - 50, 15, GRAY);
            } break;

            case GAMEPLAY: {
                // (Desenho do Gameplay - Oculto para encurtar, sem alterações)
                if (listaJogadores->head != NULL) {
                    Jogador* temp = listaJogadores->head;
                    do {
                        DrawCircleV(temp->posTela, 20, temp->cor);
                        DrawText(temp->nome, temp->posTela.x - 30, temp->posTela.y + 25, 15, BLACK);
                        temp = temp->prox;
                    } while (temp != listaJogadores->head);
                }
                if (batataAtual != NULL) {
                    Texture2D texBatataAtual;
                    
                    if (timerQueimou > 0) {
                        texBatataAtual = texBatataQueimou;
                    } else {
                        texBatataAtual = texAnimBatata[frameAnimBatata];
                    }

                    Rectangle sourceRect = { 0, 0, (float)texBatataAtual.width, (float)texBatataAtual.height };
                    Rectangle destRect = {
                        batataAtual->posTela.x,
                        batataAtual->posTela.y,
                        texBatataAtual.width * ESCALA_BATATA,
                        texBatataAtual.height * ESCALA_BATATA
                    };
                    Vector2 origin = {
                        (texBatataAtual.width * ESCALA_BATATA) / 2.0f,
                        (texBatataAtual.height * ESCALA_BATATA) / 2.0f
                    };
                    
                    DrawTexturePro(texBatataAtual, sourceRect, destRect, origin, 0.0f, WHITE);
                }

                DrawText(TextFormat("Tempo: %.1f s", (timerMusica > 0) ? timerMusica : 0.0f), 10, 10, TAMANHO_FONTE, DARKGRAY);

                if (timerQueimou > 0.0f) {
                    DrawText("QUEIMOU!", centroTela.x - MeasureText("QUEIMOU!", 60) / 2, centroTela.y - 30, 60, MAROON);
                    DrawText(batataAtual->nome, centroTela.x - MeasureText(batataAtual->nome, 30) / 2, centroTela.y + 40, 30, MAROON);
                }
            } break;

            case END_GAME: {
                // (Desenho do Fim de Jogo - Oculto para encurtar, sem alterações)
                const char* vencedor = listaJogadores->head->nome;
                DrawText("FIM DE JOGO!", centroTela.x - MeasureText("FIM DE JOGO!", 40) / 2, 50, 40, DARKGRAY);
                DrawText(TextFormat("O VENCEDOR É: %s", vencedor), centroTela.x - MeasureText(TextFormat("O VENCEDOR É: %s", vencedor), 30) / 2, 100, 30, GOLD);

                int imgPosY = 180;
                DrawTexture(texMenu, 50, imgPosY, WHITE);

                int rankingPosX = 50 + texMenu.width + 50;
                DrawText("Ordem de Eliminacao:", rankingPosX, imgPosY, 20, DARKGRAY);
                DrawText("(Do ultimo pro primeiro)", rankingPosX, imgPosY + 25, 20, GRAY);
                
                int listPosY = imgPosY + 60;
                for (int i = 0; i < placarIndex - 1; i++) {
                     DrawText(TextFormat("%d. %s", (placarIndex - 1 - i), placarEliminacao[i]), rankingPosX + 20, listPosY + (i * 30), 20, GRAY);
                }

                DrawText("Pressione ENTER para voltar ao Menu", centroTela.x - MeasureText("Pressione ENTER para voltar ao Menu", 20) / 2, 550, 20, GRAY);
            } break;
        }

        EndDrawing();
    }

    // --- Limpeza ---
    UnloadTexture(texMenu);
    UnloadTexture(texCustomize);
    UnloadTexture(texBatataPassando1);
    UnloadTexture(texBatataPassando2);
    UnloadTexture(texBatataPassando3);
    UnloadTexture(texBatataQueimou);
    
    while (getTamanho(listaJogadores) > 0) {
        removerJogador(listaJogadores, listaJogadores->head);
    }
    free(listaJogadores);
    CloseWindow();

    return 0;
}