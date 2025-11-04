#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    int numJogadores, jogadorAtual = 0, passos;
    
    printf("=== JOGO DA BATATA QUENTE ===\n");
    printf("Digite o numero de jogadores: ");
    scanf("%d", &numJogadores);

    int jogadores[numJogadores]; // Vetor pra marcar quem ainda tá no jogo
    for (int i = 0; i < numJogadores; i++) {
        jogadores[i] = 1; // 1 = ativo / 0 = eliminado
    }

    srand(time(NULL)); // Inicializa o gerador de números aleatórios

    int restantes = numJogadores;
    printf("\nA batata comeca com o jogador 1.\n\n");

    while (restantes > 1) {
        passos = rand() % numJogadores + 1; // quantos pulos a batata dá
        for (int i = 0; i < passos; i++) {
            do {
                jogadorAtual = (jogadorAtual + 1) % numJogadores;
            } while (jogadores[jogadorAtual] == 0); // pula eliminados
        }

        printf("Jogador %d ficou com a batata e foi eliminado.\n", jogadorAtual + 1);
        jogadores[jogadorAtual] = 0;
        restantes--;

        // Avança para o próximo jogador ativo
        do {
            jogadorAtual = (jogadorAtual + 1) % numJogadores;
        } while (jogadores[jogadorAtual] == 0);

        printf("A batata agora esta com o jogador %d.\n\n", jogadorAtual + 1);
    }

    // Encontra o vencedor
    for (int i = 0; i < numJogadores; i++) {
        if (jogadores[i] == 1) {
            printf("O jogador %d venceu o jogo da batata quente.\n", i + 1);
            break;
        }
    }

    return 0;
}
