#ifndef UTILS_H
#define UTILS_H

#include "raylib.h"


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

#define PONTOS_POR_SEGUNDO 100.0f
#define PONTOS_BONUS_SOBREVIVENCIA 100.0f

#define PATH_MENU "Sprites/Imagens/menu principal.png"
#define PATH_B2 "Sprites/Imagens/bq2.png"
#define PATH_B4 "Sprites/Imagens/bq4.png"
#define PATH_B9 "Sprites/Imagens/bq9.png"
#define PATH_B6 "Sprites/Imagens/bq6.png"
#define PATH_B5 "Sprites/Imagens/bq5.png"

#define PATH_MUSIC "Audio/trilha.ogg"
#define PATH_SFX_PASS "Audio/pass.wav"
#define PATH_SFX_BURN "Audio/burn.wav"
#define PATH_SFX_MOVE "Audio/move.wav"
#define PATH_SFX_SELECT "Audio/select.wav"

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


bool IsEnterPressed(void);

const char* getModoTimerTexto(ModoTimer modo);

const char* getModoJogoTexto(ModoJogo modo);

float getNovoTimer(ModoTimer modo, float tempoPersonalizado, int numJogadoresInicio, int numEliminados);

#endif 