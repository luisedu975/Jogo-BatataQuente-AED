#include "game.h"

void PrepararERodarJogo(char playerNames[][TAMANHO_NOME]) {
    
    if (listaJogadores != NULL) {
        while (contarJogadores(listaJogadores) > 0) {
            removerDaRoda(listaJogadores, listaJogadores->head);
        }
    } else {
        listaJogadores = criarLista();
    }
    
    resetarPlacar(); 
    
    Vector2 centroTela = { LARGURA_TELA / 2.0f, ALTURA_TELA / 2.0f };

    for (int i = 0; i < numJogadoresAtual; i++) {
        float angulo = (float)i / numJogadoresAtual * (2 * PI);
        Vector2 pos = {
            centroTela.x + RAIO_CIRCULO * cosf(angulo),
            centroTela.y + RAIO_CIRCULO * sinf(angulo)
        };
        Color cor = { (unsigned char)GetRandomValue(100, 250),
                      (unsigned char)GetRandomValue(100, 250),
                      (unsigned char)GetRandomValue(100, 250), 255 };
        
        bool ehHumano = (modoJogoAtual == MULTIPLAYER) || (i == 0);
        
        Jogador* novo = criarJogador(playerNames[i], pos, cor, ehHumano, i);
        inserirNaRoda(listaJogadores, novo); 
    }
    
    batataAtual = listaJogadores->head;
    
    telaAtual = GAMEPLAY;
}