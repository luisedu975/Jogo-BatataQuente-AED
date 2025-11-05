CC = gcc
CFLAGS = -Wall -std=c11
LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm

all:
	$(CC) main.c -o batata $(CFLAGS) $(LIBS)