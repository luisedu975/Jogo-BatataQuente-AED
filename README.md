# 🥔 Batata Quente Tática (AED)

### 📚 Disciplina: Algoritmos e Estruturas de Dados (AED)

**Integrantes do grupo:**
* João Victor Uchôa
* Luis Eduardo Bérard
* Pedro Guerra
* Luis Guilherme Leiria

---


## 🎥 Vídeo de Demonstração

Para ver o jogo em ação, assista ao vídeo de demonstração:

**[>> https://youtu.be/40-sHfB6ZTE <<]**

---
## 🎮 Sobre o Projeto

Este trabalho é uma repaginação tática do clássico jogo "Batata Quente", desenvolvido em C com Raylib para a disciplina de Algoritmos e Estruturas de Dados.

Abandonamos a sorte pura da brincadeira original e a substituímos por uma mecânica de **"Acumulador de Risco"**. O objetivo não é apenas sobreviver, mas acumular o máximo de pontos possível antes que a música (oculta) pare, arriscando ser eliminado.

---
## 🚀 Recursos Principais

* **Menu Interativo Completo:** Navegue por 4 telas (Menu, Customização de Nomes, Jogo, e Ranking Final).
* **Modo Solo vs. Multiplayer:** Na tela de menu, escolha quantos jogadores são "Humanos" e quantos são "NPCs".
* **Gameplay Tático:** O jogo não é só sorte. Use as teclas [1] a [5] para "mirar" a batata e pular jogadores específicos na roda.
* **Timer Dinâmico:** Escolha entre 4 modos de jogo (Aleatório, Crescente, Decrescente, Personalizado) para definir a dificuldade e o ritmo das rodadas.
* **Áudio e Sprites Customizados:** O jogo inclui sprites customizadas para cada jogador, música de fundo dinâmica (que para quando alguém "queima") e efeitos sonoros para passes e eliminações.
---
## 🕹️ Como Jogar: A Repaginação "Acumulador de Risco"

O objetivo é ser o último jogador sobrevivente. O placar final é determinado pela pontuação.

### A Mecânica de Risco/Recompensa

1.  **Recompensa (Ganhar Pontos):** Enquanto um jogador segura a batata, ele acumula pontos (`+100 pontos/segundo`).
2.  **Risco (Ser Eliminado):** Um timer de rodada (oculto) está sempre correndo. Se esse timer zerar enquanto você segura a batata, você "queima" e é **eliminado**.
3.  **Bônus de Sobrevivência:** Quando um jogador é eliminado, todos os outros jogadores que sobreviveram à rodada recebem um bônus (`+100 pontos`).

### Controles Táticos (A Lista Circular em Ação)

Esta não é uma passagem comum. O jogador humano tem controle tático sobre *para quem* passar a batata:

* **Vez do Humano:** Quando a batata está com você, o jogo exibe um aviso (`VEZ DE...`).
* **Teclas [1] a [5] (ou Numpad):** Use as teclas numéricas para passar a batata.
    * **[1]** passa para o jogador 1 casa à frente.
    * **[2]** passa para o jogador 2 casas à frente (pulando 1).
    * **[3]** passa para o jogador 3 casas à frente (pulando 2).
    * ...e assim por diante.
* **Restrição:** Você não pode escolher um pulo de valor igual ou maior que o número de jogadores restantes na roda (ex: com 3 jogadores, você não pode apertar [3], pois passaria para si mesmo).

---
## 📂 Estrutura de Pastas

Para que a compilação e execução funcionem, o projeto deve seguir a seguinte estrutura de arquivos. As texturas e sons **devem** estar dentro das pastas `Sprites/Imagens/` e `Audio/`.

/Jogo-BatataQuente-AED
|
├── Sons/
│ ├── trilha.ogg
│ ├── pass.wav
│ ├── burn.wav
│ └── ...
│
├── Sprites/Imagens/
│ ├── menu principal.png
│ ├── bq2.png, bq4.png, ...
│ └── Boneco 1.png, Boneco 2.png, ...
│
├── main.c
├── jogador.c
├── jogador.h
├── placar.c
├── placar.h
├── utils.c
├── utils.h
└── README.md
 
---

## 🚀 Como Compilar e Executar

Este projeto foi desenvolvido em **C** e **Raylib**, utilizando o ambiente **MSYS2 (UCRT64)** no Windows.

### 1. Pré-requisitos (MSYS2)

Certifique-se de que você tem o `gcc` e a `raylib` instalados no seu terminal UCRT64:
```bash
pacman -S mingw-w64-ucrt-x86_64-gcc
pacman -S mingw-w64-ucrt-x86_64-raylib
```
🧩 2. Instruções de Compilação

Navegue até a pasta raiz do projeto (onde está o `main.c`) e execute o seguinte comando para compilar todos os arquivos-fonte:

```bash
gcc main.c jogador.c placar.c utils.c -o jogo.exe -Wall -Wextra $(pkg-config --cflags --libs raylib)
```
💡 Este comando usa pkg-config para linkar automaticamente a biblioteca Raylib e suas dependências.

▶️ 3. Execução
Após a compilação bem-sucedida, um arquivo jogo.exe será criado. Execute-o com:
```bash
./jogo.exe
```

## 🧠 Requisitos e Conceitos Aplicados

Este projeto cumpre todos os requisitos obrigatórios da especificação da atividade prática:

1. **Estrutura de Dados na Lógica Central**

   **Lista Circular Encadeada Simples (`jogador.c`)**  
   É o coração do jogo, armazena a "roda" de jogadores.  
   A nova mecânica de **"Passe Tático"** (pular N casas) demonstra uma travessia complexa (`jogador->prox->prox...`) e um domínio da estrutura circular.

2. **Algoritmo de Ordenação**

   **Insertion Sort (`placar.c`)**  
   Utilizado na tela **END_GAME**, ordena o placar final não por ordem alfabética ou de eliminação, mas pela pontuação final (decrescente), criando um **Ranking dos Campeões** baseado em performance.

3. **Algumas funções da Estrutura de Dados**

   - `criarRoda()`: Aloca e inicializa a lista.  
   - `inserirNaRoda()`: Adiciona um `Jogador*` no início da lista, mantendo a circularidade.  
   - `removerDaRoda()`: Remove um `Jogador*` (o "queimado") e religa os ponteiros da lista.
   - `contarJogadores()`: Retorna o tamanho da lista (usado para lógica de validação do pulo).  
   - `passarBatata()`: Demonstra a travessia de N-passos na lista circular.

4. **Outros Requisitos**

   - **Linguagem C:** O projeto é 100% em C.  
   - **Menu Interativo:** Possui quatro telas (Menu, Customização, Jogo, Ranking).  
   - **Interface Gráfica (Bônus):** Desenvolvido com a biblioteca **Raylib**.





