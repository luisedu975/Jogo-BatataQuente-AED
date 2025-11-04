#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    int numJogadores, jogadorAtual = 0, passos, i;

    printf("=== JOGO DA BATATA QUENTE ===\n");
    printf("Digite o numero de jogadores: ");
    scanf("%d", &numJogadores);

    srand(time(NULL)); // Inicializa a aleatoriedade

    printf("\nA batata comeca com o jogador 1!\n");

    // Simula a passagem da batata algumas vezes
    for (i = 0; i < 10; i++) {
        passos = rand() % numJogadores; // Quantos jogadores a batata vai pular
        jogadorAtual = (jogadorAtual + passos) % numJogadores;

        printf("A batata passou para o jogador %d!\n", jogadorAtual + 1);
    }

    printf("\nFim da rodada de testes!\n");
    return 0;
}
