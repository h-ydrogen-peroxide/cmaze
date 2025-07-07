CC = gcc
CCWEB = emcc

CFLAGS = -Wall -Wextra

SRC = src/main.c

all: desktop

desktop: $(SRC)
	$(CC) -o bin/program $(SRC) -lGL -lm -lpthread -ldl -lrt -lX11 -lraylib $(CFLAGS)

web: $(SRC)
	$(CCWEB) -o web/game.html $(SRC) $(CFLAGS) ./libraylib.a -I. -I./raylib.h -L. -s USE_GLFW=3 -s ASYNCIFY --shell-file ./minshell.html -DPLATFORM_WEB
