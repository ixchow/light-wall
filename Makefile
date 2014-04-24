.PHONY : all

all : sim

%.o : %.cpp Patterns.h Ramps.h
	g++ -c -Wall -Werror -o $@ $< -I../game-libs64/out/include/SDL2 -D_REENTRANT

sim : sim-main.o
	g++ -Wall -Werror -o $@ $< -L../game-libs64/out/lib -lSDL2 -ldl -lpthread -lGL
