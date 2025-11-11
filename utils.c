#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

bool IsEnterPressed() {
    return IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER);
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