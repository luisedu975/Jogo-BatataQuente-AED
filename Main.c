#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h> 
#include "utils.h"
#include "jogador.h"
#include "placar.h"

Texture2D texturasJogadores[MAX_JOGADORES];

// --- Protótipos das funções de utils.c ---
// (O corpo dessas funções deve estar em utils.c)
bool IsEnterPressed(void);
const char* getModoTimerTexto(ModoTimer modo);
const char* getModoJogoTexto(ModoJogo modo);
float getNovoTimer(ModoTimer modo, float tempoPersonalizado, int numJogadoresInicio, int numEliminados);


int main(void) {
    
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Batata Quente Tática");
    SetTargetFPS(60);
    srand(time(NULL));
    InitAudioDevice();
    SetMasterVolume(1.0f);

    for (int i = 0; i < MAX_JOGADORES; i++) {
        char path[100];
        sprintf(path, "Sprites/Imagens/Boneco %d.png", i + 1);
        
        if (FileExists(path)) {
            texturasJogadores[i] = LoadTexture(path);
        } else {
            printf("AVISO: Sprite %s nao encontrado!\n", path);
            texturasJogadores[i] = LoadTexture(PATH_B2); 
        }
    }

    Texture2D texMenu = LoadTexture(PATH_MENU);
    Texture2D texCustomize = LoadTexture(PATH_B5);
    Texture2D texBatataPassando1 = LoadTexture(PATH_B2);
    Texture2D texBatataPassando2 = LoadTexture(PATH_B4);
    Texture2D texBatataPassando3 = LoadTexture(PATH_B9);
    Texture2D texBatataQueimou = LoadTexture(PATH_B6);
    Texture2D texAnimBatata[3] = { texBatataPassando1, texBatataPassando2, texBatataPassando3 };
    Texture2D texFundo = LoadTexture(PATH_FUNDO);
    Music trilha;
    bool temTrilha = false;
    if (FileExists(PATH_MUSIC)) {
        trilha = LoadMusicStream(PATH_MUSIC);
        SetMusicVolume(trilha, 0.6f);
        PlayMusicStream(trilha);
        temTrilha = true;
    }
    Sound sfxPass, sfxBurn, sfxMove, sfxSelect;
    bool temSfxPass = false, temSfxBurn = false, temSfxMove = false, temSfxSelect = false;
    if (FileExists(PATH_SFX_PASS)) { sfxPass = LoadSound(PATH_SFX_PASS); SetSoundVolume(sfxPass, 0.9f); temSfxPass = true; }
    if (FileExists(PATH_SFX_BURN)) { sfxBurn = LoadSound(PATH_SFX_BURN); SetSoundVolume(sfxBurn, 1.0f); temSfxBurn = true; }
    if (FileExists(PATH_SFX_MOVE)) { sfxMove = LoadSound(PATH_SFX_MOVE); SetSoundVolume(sfxMove, 0.6f); temSfxMove = true; }
    if (FileExists(PATH_SFX_SELECT)) { sfxSelect = LoadSound(PATH_SFX_SELECT); SetSoundVolume(sfxSelect, 0.8f); temSfxSelect = true; }

    GameScreen telaAtual = MENU;
    Vector2 centroTela = { LARGURA_TELA / 2.0f, ALTURA_TELA / 2.0f };
    bool querSair = false;

    int menuSelecao = 0;
    int numJogadoresAtual = DEFAULT_JOGADORES;
    ModoTimer modoTimerAtual = ALEATORIO;
    int numHumanos = 1;
    float tempoPersonalizado = 5.0f;
    
    char playerNames[MAX_JOGADORES][TAMANHO_NOME];
    int nameBoxSelecao = 0;
    bool nameBoxAtiva = false;
    int nameCharCount = 0;
    
    ListaCircular* listaJogadores = criarRoda();
    Jogador* batataAtual = NULL;
    int frameAnimBatata = 0;
    float timerMusica = 0.0f;
    float npcPassTimer = 0.0f;
    float timerQueimou = 0.0f;
    
    bool placarFoiOrdenado = false;

    while (!WindowShouldClose() && !querSair) {
        if (temTrilha) UpdateMusicStream(trilha);

        switch (telaAtual) {
            case MENU: {
                int totalMenuOpcoes = (modoTimerAtual == PERSONALIZADO) ? 6 : 5;

                if (IsKeyPressed(KEY_DOWN)) {
                    menuSelecao = (menuSelecao + 1) % totalMenuOpcoes;
                    if (temSfxMove) PlaySound(sfxMove);
                }
                if (IsKeyPressed(KEY_UP)) {
                    menuSelecao = (menuSelecao - 1 + totalMenuOpcoes) % totalMenuOpcoes;
                    if (temSfxMove) PlaySound(sfxMove);
                }

                if (menuSelecao == 1) { // Jogadores (Total)
                    if (IsKeyPressed(KEY_RIGHT) && numJogadoresAtual < MAX_JOGADORES) { numJogadoresAtual++; if (numHumanos > numJogadoresAtual) numHumanos = numJogadoresAtual; if (temSfxMove) PlaySound(sfxMove); }
                    if (IsKeyPressed(KEY_LEFT) && numJogadoresAtual > MIN_JOGADORES) { numJogadoresAtual--; if (numHumanos > numJogadoresAtual) numHumanos = numJogadoresAtual; if (temSfxMove) PlaySound(sfxMove); }
                }
                // --- AJUSTE: Lógica do Menu Corrigida ---
                else if (menuSelecao == 2) { // Jogadores Humanos
                    if (IsKeyPressed(KEY_RIGHT) && numHumanos < numJogadoresAtual) { numHumanos++; if (temSfxMove) PlaySound(sfxMove); }
                    if (IsKeyPressed(KEY_LEFT) && numHumanos > 1) { numHumanos--; if (temSfxMove) PlaySound(sfxMove); }
                }
                else if (menuSelecao == 3) { // Modo Timer
                    if (IsKeyPressed(KEY_RIGHT)) { modoTimerAtual = (modoTimerAtual + 1) % 4; if (temSfxMove) PlaySound(sfxMove); }
                    if (IsKeyPressed(KEY_LEFT)) { modoTimerAtual = (modoTimerAtual - 1 + 4) % 4; if (temSfxMove) PlaySound(sfxMove); }
                }
                else if (menuSelecao == 4 && modoTimerAtual == PERSONALIZADO) { // Tempo Fixo
                    if (IsKeyPressed(KEY_RIGHT) && tempoPersonalizado < 20.0f) { tempoPersonalizado += 0.5f; if (temSfxMove) PlaySound(sfxMove); }
                    if (IsKeyPressed(KEY_LEFT) && tempoPersonalizado > 1.0f) { tempoPersonalizado -= 0.5f; if (temSfxMove) PlaySound(sfxMove); }
                }
                else if (IsEnterPressed()) { 
                    int acaoSair = (modoTimerAtual == PERSONALIZADO) ? 5 : 4;
                    if (menuSelecao == 0) { 
                        if (temSfxSelect) PlaySound(sfxSelect);
                        for (int i = 0; i < numJogadoresAtual; i++) {
                            sprintf(playerNames[i], "Jogador %d", i + 1);
                        }
                        nameBoxSelecao = 0;
                        nameBoxAtiva = false;
                        nameCharCount = 0;
                        telaAtual = CUSTOMIZE_NAMES;
                    }
                    else if (menuSelecao == acaoSair) { 
                        if (temSfxSelect) PlaySound(sfxSelect);
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
                        if (temSfxMove) PlaySound(sfxMove);
                    }
                    if (IsKeyPressed(KEY_UP)) {
                        nameBoxSelecao = (nameBoxSelecao - 1 + totalOpcoesNome) % totalOpcoesNome;
                        if (temSfxMove) PlaySound(sfxMove);
                    }

                    if (IsEnterPressed()) {
                        if (nameBoxSelecao == numJogadoresAtual) { 
                            if (temSfxSelect) PlaySound(sfxSelect);
                            
                            destruirRoda(listaJogadores);
                            listaJogadores = criarRoda();
                            resetarPlacar(); 
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
                                
                                bool ehHumano = (i < numHumanos);
                                
                                Jogador* novo = criarJogador(playerNames[i], pos, cor, ehHumano, i); 
                                inserirNaRoda(listaJogadores, novo);
                            }
                            
                            batataAtual = listaJogadores->head;
                            timerMusica = getNovoTimer(modoTimerAtual, tempoPersonalizado, numJogadoresAtual, placarIndex);
                            
                            if (!batataAtual->ehHumano) {
                                npcPassTimer = (float)GetRandomValue(20, 150) / 100.0f;
                            }
                            
                            telaAtual = GAMEPLAY;
                        
                        } else { 
                            if (temSfxSelect) PlaySound(sfxSelect);
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
                        
                        adicionarAoPlacar(jogadorEliminado->nome, jogadorEliminado->pontuacao);

                        if (contarJogadores(listaJogadores) > 2) 
                        {
                            Jogador* temp = listaJogadores->head;
                            do {
                                if (temp != jogadorEliminado) {
                                    temp->pontuacao += PONTOS_BONUS_SOBREVIVENCIA;
                                }
                                temp = temp->prox;
                            } while (temp != listaJogadores->head);
                        }
                        
                        batataAtual = removerDaRoda(listaJogadores, jogadorEliminado); 
                        
                        if (contarJogadores(listaJogadores) == 1) { 
                            adicionarAoPlacar(listaJogadores->head->nome, listaJogadores->head->pontuacao); 
                            telaAtual = END_GAME;
                        } else {
                            timerMusica = getNovoTimer(modoTimerAtual, tempoPersonalizado, numJogadoresAtual, placarIndex);
                            if (batataAtual != NULL && !batataAtual->ehHumano) {
                                npcPassTimer = (float)GetRandomValue(20, 150) / 100.0f;
                            }
                        }
                    }
                } else {
                    
                    timerMusica -= GetFrameTime();

                    if (batataAtual != NULL) {
                        batataAtual->pontuacao += (PONTOS_POR_SEGUNDO * GetFrameTime());
                    }

                    if (timerMusica <= 0.0f) {
                        timerQueimou = 2.0f; 
                        if (temSfxBurn) PlaySound(sfxBurn);
                    }
                    
                    // --- AJUSTE: Lógica de Pulo Tático ---
                    if (batataAtual != NULL && batataAtual->ehHumano) {
                        
                        int passos = 0;
                        if (IsKeyPressed(KEY_ONE)   || IsKeyPressed(KEY_KP_1)) passos = 1;
                        if (IsKeyPressed(KEY_TWO)   || IsKeyPressed(KEY_KP_2)) passos = 2;
                        if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)) passos = 3;
                        if (IsKeyPressed(KEY_FOUR)  || IsKeyPressed(KEY_KP_4)) passos = 4;
                        if (IsKeyPressed(KEY_FIVE)  || IsKeyPressed(KEY_KP_5)) passos = 5;

                        if (passos > 0) {
                            // Validação: Não pular um N >= ao total de jogadores
                            if (passos < contarJogadores(listaJogadores)) {
                                
                                if (temSfxPass) {
                                    SetSoundPitch(sfxPass, 0.95f + (GetRandomValue(0, 10) / 100.0f));
                                    PlaySound(sfxPass);
                                }
                                
                                batataAtual = passarBatata(batataAtual, passos); // <-- CHAMA A FUNÇÃO CORRETA
                                frameAnimBatata = (frameAnimBatata + 1) % 3;
                                
                                if (!batataAtual->ehHumano) {
                                    npcPassTimer = (float)GetRandomValue(20, 150) / 100.0f;
                                }
                            }
                        }
                    } 
                    else if (batataAtual != NULL && !batataAtual->ehHumano) {
                        npcPassTimer -= GetFrameTime();
                        if (npcPassTimer <= 0.0f) {
                            if (temSfxPass) {
                                SetSoundPitch(sfxPass, 0.95f + (GetRandomValue(0, 10) / 100.0f));
                                PlaySound(sfxPass);
                            }
                            batataAtual = passarBatata(batataAtual, 1); // NPC sempre passa 1
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
                    insertionSortPlacar(); 
                    placarFoiOrdenado = true;
                }
                if (IsEnterPressed()) {
                    if (temSfxSelect) PlaySound(sfxSelect);
                    telaAtual = MENU;
                }
                if (IsKeyPressed(KEY_ESCAPE)) {
                    querSair = true;
                }
            } break;
        }

        BeginDrawing();
        DrawTexture(texFundo, 0, 0, WHITE);
        Color azulSuave = {135, 206, 250, 245};
        switch (telaAtual) {
            case MENU: ClearBackground(azulSuave); break;
            case CUSTOMIZE_NAMES: ClearBackground(LIGHTGRAY); break;
            case GAMEPLAY: ClearBackground(BEIGE); break;
            case END_GAME: ClearBackground(DARKGRAY); break;
        }

        switch (telaAtual) {
            case MENU: {
                Rectangle sourceRect = { 0, 0, (float)texMenu.width, (float)texMenu.height };
                Rectangle destRect = { centroTela.x, 180, texMenu.width * ESCALA_MENU, texMenu.height * ESCALA_MENU };
                Vector2 origin = { (texMenu.width * ESCALA_MENU) / 2.0f, (texMenu.height * ESCALA_MENU) / 2.0f };
                DrawTexturePro(texMenu, sourceRect, destRect, origin, 0.0f, WHITE);
                
                int posY = destRect.y + origin.y + 15;
                
                DrawText("Iniciar", centroTela.x - MeasureText("Iniciar", 30) / 2, posY, 30, (menuSelecao == 0) ? MAROON : DARKGRAY);
                posY += 40;
                DrawText(TextFormat("Jogadores: < %d >", numJogadoresAtual), centroTela.x - MeasureText(TextFormat("Jogadores: < %d >", numJogadoresAtual), 30) / 2, posY, 30, (menuSelecao == 1) ? MAROON : DARKGRAY);
                posY += 40;
                DrawText(TextFormat("Jogadores Humanos: < %d >", numHumanos), centroTela.x - MeasureText(TextFormat("Jogadores Humanos: < %d >", numHumanos), 30) / 2, posY, 30, (menuSelecao == 2) ? MAROON : DARKGRAY);
                posY += 40;
                DrawText(TextFormat("Modo Timer: < %s >", getModoTimerTexto(modoTimerAtual)), centroTela.x - MeasureText(TextFormat("Modo Timer: < %s >", getModoTimerTexto(modoTimerAtual)), 30) / 2, posY, 30, (menuSelecao == 3) ? MAROON : DARKGRAY);
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
                    
                    const char* tipo = (i < numHumanos) ? "(Humano)" : "(NPC)";
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
                if (listaJogadores->head != NULL) {
                    Jogador* temp = listaJogadores->head;
                    do {
                        desenharJogadorNaTela(temp); 
                        temp = temp->prox;
                    } while (temp != listaJogadores->head);
                }

                if (batataAtual != NULL) {
                    Texture2D texBatataAtual = (timerQueimou > 0) ? texBatataQueimou : texAnimBatata[frameAnimBatata];
                    
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

                if (timerQueimou > 0.0f) {
                    DrawText("QUEIMOU!", centroTela.x - MeasureText("QUEIMOU!", 60) / 2, centroTela.y - 30, 60, MAROON);
                    DrawText(batataAtual->nome, centroTela.x - MeasureText(batataAtual->nome, 30) / 2, centroTela.y + 40, 30, MAROON);
                }
                
                // --- AJUSTE: Texto de ajuda Tático Dinâmico e Estático ---
                if (batataAtual != NULL && batataAtual->ehHumano && timerQueimou <= 0.0f) {
                    DrawText(TextFormat("VEZ DE %s!", TextToUpper(batataAtual->nome)), centroTela.x - MeasureText(TextFormat("VEZ DE %S!", TextToUpper(batataAtual->nome)), 40) / 2, centroTela.y - 100, 40, RED);
                    DrawText("PASSE A BATATA!", centroTela.x - MeasureText("PASSE A BATATA!", 20) / 2, centroTela.y - 60, 20, RED);
                }
                
                DrawText("Pressione a barra de espaço para passar a bomba.", 10, ALTURA_TELA - 35, 10, BLACK);
                DrawText("Para realizar um passe tático, utilize as teclas numéricas de 1 a 5, sendo que o número escolhido define quantos jogadores serão pulados.", 10, ALTURA_TELA - 15, 8, BLACK);

            } break;

            case END_GAME: {
                const char* vencedor = placarEliminacao[0].nome;
                float pontuacaoVencedor = placarEliminacao[0].pontuacao;
                
                DrawText("FIM DE JOGO!", centroTela.x - MeasureText("FIM DE JOGO!", 40) / 2, 50, 40, LIGHTGRAY);
                DrawText(TextFormat("O VENCEDOR É: %s (%.0f Pontos)", vencedor, pontuacaoVencedor), centroTela.x - MeasureText(TextFormat("O VENCEDOR É: %s(%.0f Pontos)", vencedor, pontuacaoVencedor), 30) / 2, 100, 30, GOLD);
                int rankingPosX = (int)centroTela.x;
                int rankingPosY = 200; 
                
                DrawText("Ranking Final (Por Pontos):", rankingPosX - MeasureText("Ranking Final (Por Pontos):", 20) / 2, rankingPosY, 20, LIGHTGRAY);
                
                int listPosY = rankingPosY + 40;
                
                for (int i = 1; i < placarIndex; i++) {
                     DrawText(TextFormat("  %d. %s - %.0f pts", (i + 1), placarEliminacao[i].nome, placarEliminacao[i].pontuacao), rankingPosX - 150, listPosY + ((i-1) * 30), 20, LIGHTGRAY);
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
    
    destruirRoda(listaJogadores); 
    for (int i = 0; i < MAX_JOGADORES; i++) {
        UnloadTexture(texturasJogadores[i]);
    }
    if (temSfxPass) UnloadSound(sfxPass);
    if (temSfxBurn) UnloadSound(sfxBurn);
    if (temSfxMove) UnloadSound(sfxMove);
    if (temSfxSelect) UnloadSound(sfxSelect);
    if (temTrilha) UnloadMusicStream(trilha);
    
    CloseAudioDevice();
    CloseWindow();

    return 0;
}