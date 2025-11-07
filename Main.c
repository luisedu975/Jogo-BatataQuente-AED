#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h> 

#define LARGURA_TELA 800
#define ALTURA_TELA 600

#define MIN_JOGADORES 2
#define MAX_JOGADORES 12
#define DEFAULT_JOGADORES 6

#define RAIO_CIRCULO 200
#define TAMANHO_FONTE 20
#define TAMANHO_NOME 20 

#define ESCALA_BATATA 0.25f
#define ESCALA_MENU 0.5f 

#define PATH_MENU "Sprites/Imagens/menu principal.png"
#define PATH_B2 "Sprites/Imagens/bq2.png"
#define PATH_B4 "Sprites/Imagens/bq4.png"
#define PATH_B9 "Sprites/Imagens/bq9.png"
#define PATH_B6 "Sprites/Imagens/bq6.png"
#define PATH_B5 "Sprites/Imagens/bq5.png"


typedef enum GameScreen {
    MENU,
    CUSTOMIZE_NAMES,
    GAMEPLAY,
    END_GAME
} GameScreen;

typedef enum {
    ALEATORIO,     
    CRESCENTE,     
    DECRESCENTE,   
    PERSONALIZADO  
} ModoTimer;

typedef enum {
    SOLO,         
    MULTIPLAYER   
} ModoJogo;

bool IsEnterPressed() {
    return IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER);
}

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

ListaCircular* criarLista() {
    ListaCircular* lista = (ListaCircular*)malloc(sizeof(ListaCircular));
    if (lista != NULL) {
        lista->head = NULL;
        lista->tail = NULL;
        lista->tamanho = 0;
    }
    return lista;
}

void inserirJogador(ListaCircular* lista, const char* nome, Vector2 pos, Color cor, bool ehHumano) {
    Jogador* novo = (Jogador*)malloc(sizeof(Jogador));
    if (novo == NULL) return;
    strncpy(novo->nome, nome, TAMANHO_NOME - 1);
    novo->nome[TAMANHO_NOME - 1] = '\0';
    novo->posTela = pos;
    novo->cor = cor;
    novo->ehHumano = ehHumano;
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

void desenharJogador(Jogador* j) {
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
}


char placarEliminacao[MAX_JOGADORES][TAMANHO_NOME];
int placarIndex = 0;

void adicionarAoPlacar(const char* nome) {
    if (placarIndex < MAX_JOGADORES) {
        strcpy(placarEliminacao[placarIndex], nome);
        placarIndex++;
    }
}

void ordenarPlacar() {
    int i, j;
    char chave[TAMANHO_NOME];
    int n = placarIndex - 1; 
    for (i = 1; i < n; i++) {
        strcpy(chave, placarEliminacao[i]);
        j = i - 1;
        while (j >= 0 && strcmp(placarEliminacao[j], chave) > 0) {
            strcpy(placarEliminacao[j + 1], placarEliminacao[j]);
            j = j - 1;
        }
        strcpy(placarEliminacao[j + 1], chave);
    }
}



const char* getModoTimerTexto(ModoTimer modo) {
    switch (modo) {
        case CRESCENTE: return "Crescente";
        case DECRESCENTE: return "Decrescente";
        case PERSONALIZADO: return "Personalizado";
        case ALEATORIO: default: return "Aleatorio";
    }
}

const char* getModoJogoTexto(ModoJogo modo) {
    switch (modo) {
        case MULTIPLAYER: return "Multiplayer";
        case SOLO: default: return "Solo";
    }
}

float getNovoTimer(ModoTimer modo, float tempoPersonalizado, int numJogadoresInicio, int numEliminados) {
    if (modo == PERSONALIZADO) {
        return tempoPersonalizado;
    }
    float minTime, maxTime;
    int totalRodadas = numJogadoresInicio - 1;
    int rodadaAtual = numEliminados;
    if (totalRodadas <= 0) totalRodadas = 1; 
    switch (modo) {
        case CRESCENTE:
            minTime = 1.5f + (5.5f * ((float)rodadaAtual / totalRodadas));
            maxTime = 2.5f + (6.5f * ((float)rodadaAtual / totalRodadas));
            break;
        case DECRESCENTE:
            minTime = 7.0f - (5.5f * ((float)rodadaAtual / totalRodadas));
            maxTime = 9.0f - (6.5f * ((float)rodadaAtual / totalRodadas));
            break;
        case ALEATORIO:
        default:
            minTime = 3.0f;
            maxTime = 8.0f;
            break;
    }
    if (minTime < 1.0f) minTime = 1.0f;
    if (maxTime <= minTime) maxTime = minTime + 1.0f;
    return (float)GetRandomValue((int)(minTime * 100), (int)(maxTime * 100)) / 100.0f;
}



int main(void) {
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Batata Quente Circular (AED)");
    SetTargetFPS(60);
    srand(time(NULL));

    Texture2D texMenu = LoadTexture(PATH_MENU);
    Texture2D texCustomize = LoadTexture(PATH_B5);
    Texture2D texBatataPassando1 = LoadTexture(PATH_B2);
    Texture2D texBatataPassando2 = LoadTexture(PATH_B4);
    Texture2D texBatataPassando3 = LoadTexture(PATH_B9);
    Texture2D texBatataQueimou = LoadTexture(PATH_B6);

    Texture2D texAnimBatata[3] = { texBatataPassando1, texBatataPassando2, texBatataPassando3 };
    int frameAnimBatata = 0;

    GameScreen telaAtual = MENU;
    
    int menuSelecao = 0;
    int numJogadoresAtual = DEFAULT_JOGADORES;
    ModoTimer modoTimerAtual = ALEATORIO;
    ModoJogo modoJogoAtual = SOLO;
    float tempoPersonalizado = 5.0f;
    int totalMenuOpcoes = 5; 
    bool querSair = false;

    char playerNames[MAX_JOGADORES][TAMANHO_NOME];
    int nameBoxSelecao = 0;
    bool nameBoxAtiva = false;
    int nameCharCount = 0;
    
    ListaCircular* listaJogadores = criarLista();
    Jogador* batataAtual = NULL;
    
    float timerMusica = 0.0f;
    float npcPassTimer = 0.0f;
    float timerQueimou = 0.0f;
    
    bool placarFoiOrdenado = false;

    Vector2 centroTela = { LARGURA_TELA / 2.0f, ALTURA_TELA / 2.0f };

    while (!WindowShouldClose() && !querSair) {

        switch (telaAtual) {
            case MENU: {
                totalMenuOpcoes = (modoTimerAtual == PERSONALIZADO) ? 6 : 5;

                if (IsKeyPressed(KEY_DOWN)) {
                    menuSelecao = (menuSelecao + 1) % totalMenuOpcoes;
                }
                if (IsKeyPressed(KEY_UP)) {
                    menuSelecao = (menuSelecao - 1 + totalMenuOpcoes) % totalMenuOpcoes;
                }

                if (menuSelecao == 1) { 
                    if (IsKeyPressed(KEY_RIGHT) && numJogadoresAtual < MAX_JOGADORES) {
                        numJogadoresAtual++;
                    }
                    if (IsKeyPressed(KEY_LEFT) && numJogadoresAtual > MIN_JOGADORES) {
                        numJogadoresAtual--;
                    }
                } 
                else if (menuSelecao == 2) {
                    if (IsKeyPressed(KEY_RIGHT)) {
                        modoTimerAtual = (modoTimerAtual + 1) % 4;
                    }
                    if (IsKeyPressed(KEY_LEFT)) {
                        modoTimerAtual = (modoTimerAtual - 1 + 4) % 4;
                    }
                } 
                else if (menuSelecao == 3) { 
                    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_LEFT)) {
                        modoJogoAtual = (modoJogoAtual == SOLO) ? MULTIPLAYER : SOLO;
                    }
                }
                else if (menuSelecao == 4 && modoTimerAtual == PERSONALIZADO) { 
                    if (IsKeyPressed(KEY_RIGHT) && tempoPersonalizado < 20.0f) {
                        tempoPersonalizado += 0.5f;
                    }
                    if (IsKeyPressed(KEY_LEFT) && tempoPersonalizado > 1.0f) {
                        tempoPersonalizado -= 0.5f;
                    }
                }
                else if (IsEnterPressed()) {
                    int acaoSair = (modoTimerAtual == PERSONALIZADO) ? 5 : 4;

                    if (menuSelecao == 0) { 
                        for (int i = 0; i < numJogadoresAtual; i++) {
                            sprintf(playerNames[i], "Jogador %d", i + 1);
                        }
                        nameBoxSelecao = 0;
                        nameBoxAtiva = false;
                        nameCharCount = 0;
                        telaAtual = CUSTOMIZE_NAMES;
                    }
                    else if (menuSelecao == acaoSair) { 
                        querSair = true;
                    }
                }
            } break;

            case CUSTOMIZE_NAMES: {
                int totalOpcoesNome = numJogadoresAtual + 1; 

                if (nameBoxAtiva) {
                    int key = GetCharPressed();
                    while (key > 0) {
                        if ((key >= 32) && (key <= 125) && (nameCharCount < TAMANHO_NOME - 1)) {
                            playerNames[nameBoxSelecao][nameCharCount] = (char)key;
                            nameCharCount++;
                            playerNames[nameBoxSelecao][nameCharCount] = '\0';
                        }
                        key = GetCharPressed();
                    }
                    if (IsKeyPressedRepeat(KEY_BACKSPACE)) {
                        if (nameCharCount > 0) {
                            nameCharCount--;
                            playerNames[nameBoxSelecao][nameCharCount] = '\0';
                        }
                    }
                    if (IsEnterPressed() || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_UP)) {
                        nameBoxAtiva = false;
                    }
                } 
                else {
                    if (IsKeyPressed(KEY_DOWN)) {
                        nameBoxSelecao = (nameBoxSelecao + 1) % totalOpcoesNome;
                    }
                    if (IsKeyPressed(KEY_UP)) {
                        nameBoxSelecao = (nameBoxSelecao - 1 + totalOpcoesNome) % totalOpcoesNome;
                    }

                    if (IsEnterPressed()) {
                        if (nameBoxSelecao == numJogadoresAtual) {
                            while (getTamanho(listaJogadores) > 0) {
                                removerJogador(listaJogadores, listaJogadores->head);
                            }
                            placarIndex = 0;
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
                                
                               
                                bool ehHumano = false;
                                if (modoJogoAtual == MULTIPLAYER) {
                                    ehHumano = true; 
                                } else {
                                    ehHumano = (i == 0); 
                                }
                                
                                inserirJogador(listaJogadores,  playerNames[i], pos, cor, ehHumano); 
                            }
                            
                            batataAtual = listaJogadores->head;
                            timerMusica = getNovoTimer(modoTimerAtual, tempoPersonalizado, numJogadoresAtual, placarIndex);
                            
                            if (!batataAtual->ehHumano) {
                                npcPassTimer = (float)GetRandomValue(20, 150) / 100.0f;
                            }
                            
                            telaAtual = GAMEPLAY;
                        
                        } else {
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
                        adicionarAoPlacar(jogadorEliminado->nome);
                        
                        batataAtual = batataAtual->prox; 
                        removerJogador(listaJogadores, jogadorEliminado);
                        imprimirListaConsole(listaJogadores);

                        if (getTamanho(listaJogadores) == 1) {
                            adicionarAoPlacar(listaJogadores->head->nome);
                            telaAtual = END_GAME;
                        } else {
                            timerMusica = getNovoTimer(modoTimerAtual, tempoPersonalizado, numJogadoresAtual, placarIndex);
                            if (!batataAtual->ehHumano) {
                                npcPassTimer = (float)GetRandomValue(20, 150) / 100.0f;
                            }
                        }
                    }
                } else { 
                    timerMusica -= GetFrameTime();

                    if (timerMusica <= 0.0f) {
                        timerQueimou = 2.0f;
                    }
                    
                    if (batataAtual->ehHumano) {
                        if (IsKeyPressed(KEY_SPACE)) {
                            batataAtual = batataAtual->prox;
                            frameAnimBatata = (frameAnimBatata + 1) % 3;
                            
                            if (!batataAtual->ehHumano) {
                                npcPassTimer = (float)GetRandomValue(20, 150) / 100.0f;
                            }
                        }
                    } else {
                        npcPassTimer -= GetFrameTime();
                        
                        if (npcPassTimer <= 0.0f) {
                            batataAtual = batataAtual->prox;
                            frameAnimBatata = (frameAnimBatata + 1) % 3;
                            
                            if (!batataAtual->ehHumano) {
                                npcPassTimer = (float)GetRandomValue(20, 150) / 100.0f;
                            }
                        }
                    }
                }
            } break;

            case END_GAME: {
                if (!placarFoiOrdenado) {
                    placarFoiOrdenado = true;
                }
                if (IsEnterPressed()) {
                    telaAtual = MENU;
                }
            } break;
        }

        BeginDrawing();
        Color azulSuave = {135, 206, 250, 245};
        switch (telaAtual) {
            case MENU:
                ClearBackground(azulSuave); 
                break;
            case CUSTOMIZE_NAMES:
                ClearBackground(LIGHTGRAY); 
                break;
            case GAMEPLAY:
                ClearBackground(BEIGE); 
                break;
            case END_GAME:
                ClearBackground(DARKGRAY); 
                break;
        }


        switch (telaAtual) {
            case MENU: {
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
                
                int posY = destRect.y + origin.y - 40; 
                
                DrawText("Iniciar", centroTela.x - MeasureText("Iniciar", 30) / 2, posY, 30, (menuSelecao == 0) ? MAROON : DARKGRAY);
                posY += 40;
                DrawText(TextFormat("Jogadores: < %d >", numJogadoresAtual), centroTela.x - MeasureText(TextFormat("Jogadores: < %d >", numJogadoresAtual), 30) / 2, posY, 30, (menuSelecao == 1) ? MAROON : DARKGRAY);
                posY += 40;
                DrawText(TextFormat("Modo Timer: < %s >", getModoTimerTexto(modoTimerAtual)), centroTela.x - MeasureText(TextFormat("Modo Timer: < %s >", getModoTimerTexto(modoTimerAtual)), 30) / 2, posY, 30, (menuSelecao == 2) ? MAROON : DARKGRAY);
                posY += 40;
                DrawText(TextFormat("Modo de Jogo: < %s >", getModoJogoTexto(modoJogoAtual)), centroTela.x - MeasureText(TextFormat("Modo de Jogo: < %s >", getModoJogoTexto(modoJogoAtual)), 30) / 2, posY, 30, (menuSelecao == 3) ? MAROON : DARKGRAY);
                posY += 40;

                int acaoSair = 4;
                if (modoTimerAtual == PERSONALIZADO) {
                    DrawText(TextFormat("Tempo Fixo: < %.1f s >", tempoPersonalizado), centroTela.x - MeasureText(TextFormat("Tempo Fixo: < %.1f s >", tempoPersonalizado), 30) / 2, posY, 30, (menuSelecao == 4) ? MAROON : DARKGRAY);
                    posY += 40;
                    acaoSair = 5;
                }
                DrawText("Sair", centroTela.x - MeasureText("Sair", 30) / 2, posY, 30, (menuSelecao == acaoSair) ? MAROON : DARKGRAY);
                
                DrawText("Use SETAS (CIMA/BAIXO/LADOS) para ajustar", 10, ALTURA_TELA - 30, 20, GRAY);
            } break;

            case CUSTOMIZE_NAMES: {
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
                    
                    const char* tipo = (modoJogoAtual == MULTIPLAYER || i == 0) ? "(Humano)" : "(NPC)";
                    DrawText(TextFormat("%s %s", playerNames[i], tipo), textBox.x + 5, textBox.y + 7, 20, BLACK);

                    if (nameBoxAtiva && nameBoxSelecao == i && ((int)(GetTime() * 2) % 2 == 0)) {
                        float textWidth = MeasureText(playerNames[i], 20);
                        DrawText("|", textBox.x + 5 + textWidth, textBox.y + 7, 20, MAROON);
                    }
                }
                
                int posYBotao = 80 + numJogadoresAtual * 40;
                DrawText("Confirmar e Iniciar", textBoxPosX + 150 - MeasureText("Confirmar e Iniciar", 20) / 2, posYBotao, 20, (nameBoxSelecao == numJogadoresAtual) ? MAROON : DARKGRAY);
                
                float escala = 0.86f;
                float imgX = LARGURA_TELA - texCustomize.width * escala + 22;
                float imgY = centroTela.y - texCustomize.height * escala / 1.4f + 83;
                DrawTextureEx(texCustomize, (Vector2){imgX, imgY}, 0.0f, escala, WHITE);

                DrawText("Use CIMA/BAIXO, ENTER para editar, BACKSPACE para apagar. ESC para voltar.", 10, ALTURA_TELA - 50, 15, GRAY);
            } break;

            case GAMEPLAY: {
                // (Desenho do Gameplay)
                if (listaJogadores->head != NULL) {
                    Jogador* temp = listaJogadores->head;
                    do {
                        desenharJogador(temp);
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
                
                if (batataAtual != NULL && batataAtual->ehHumano && timerQueimou <= 0.0f) {
                    DrawText(TextFormat("VEZ DE %s!", TextToUpper(batataAtual->nome)), centroTela.x - MeasureText(TextFormat("VEZ DE %s!", TextToUpper(batataAtual->nome)), 40) / 2, centroTela.y - 100, 40, RED);
                    DrawText("Pressione ESPAÇO para passar!", centroTela.x - MeasureText("Pressione ESPAÇO para passar!", 20) / 2, centroTela.y - 60, 20, RED);
                }
                
            } break;

            case END_GAME: {
                const char* vencedor = listaJogadores->head->nome;
                
                DrawText("FIM DE JOGO!", centroTela.x - MeasureText("FIM DE JOGO!", 40) / 2, 50, 40, LIGHTGRAY);
                DrawText(TextFormat("O VENCEDOR É: %s", vencedor), centroTela.x - MeasureText(TextFormat("O VENCEDOR É: %s", vencedor), 30) / 2, 100, 30, GOLD);

                int rankingPosX = (int)centroTela.x;
                int rankingPosY = 180; 
                
                DrawText("Ordem de Eliminação:", rankingPosX - MeasureText("Ordem de Eliminação:", 20) / 2, rankingPosY, 20, LIGHTGRAY);
                
                int listPosY = rankingPosY + 40;
                
                for (int i = 0; i < placarIndex - 1; i++) {
                     DrawText(TextFormat("%dº Eliminado: %s", (i + 1), placarEliminacao[i]), rankingPosX - 100, listPosY + (i * 30), 20, LIGHTGRAY);
                }

                DrawText("Pressione ENTER para voltar ao Menu", centroTela.x - MeasureText("Pressione ENTER para voltar ao Menu", 20) / 2, 550, 20, LIGHTGRAY);
            } break;
        }

        EndDrawing();
    }

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