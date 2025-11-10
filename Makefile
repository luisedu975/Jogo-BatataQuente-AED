EXECUTAVEL = jogo.exe
FONTES = main.c jogador.c placar.c utils.c

CFLAGS = -Wall -Wextra
LDFLAGS = -lraylib -lglfw -lopengl32 -lwinmm -lgdi32 -lshell32

all: $(FONTES)
	gcc $(FONTES) -o $(EXECUTAVEL) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(EXECUTAVEL)