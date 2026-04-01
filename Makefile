CC     = gcc
CFLAGS = -O2 -Wall -Wextra -ansi -Iinclude/ -IC:/msys64/ucrt64/include/SDL
CLIBS  = -lSDL_gfx -lSDLmain -lSDL -lm -mwindows

EXE  = jeu.exe
SRC  = src/
INC  = include/

FILEO := config.o physics.o ui.o game.o main.o

$(EXE) : $(FILEO)
	$(CC) $(CFLAGS) -o $@ $^ $(CLIBS)
	rm -f $(FILEO)

main.o : $(SRC)main.c $(INC)game.h
	$(CC) $(CFLAGS) -o $@ -c $<

game.o : $(SRC)game.c $(INC)game.h $(INC)types.h $(INC)config.h $(INC)physics.h $(INC)ui.h
	$(CC) $(CFLAGS) -o $@ -c $<

%.o : $(SRC)%.c $(INC)%.h
	$(CC) $(CFLAGS) -o $@ -c $<

clean :
	rm -f $(FILEO)
	rm -f $(EXE)

.PHONY: clean