CC     = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -Iinclude

SDL_INC = -IC:/msys64/ucrt64/include/SDL2
SDL_LIB = -LC:/msys64/ucrt64/lib -lSDL2_ttf -lSDL2main -lSDL2
LIBS    = -lm $(SDL_LIB)

SRC    = src/main.c src/ui.c src/physics.c src/config.c
TARGET = jeu.exe

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SDL_INC) -o $@ $^ $(LIBS)
	@echo "OK : ./$(TARGET)"

clean:
	rm -f $(TARGET)

.PHONY: all clean