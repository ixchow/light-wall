.PHONY : all clean

UNAME := $(shell uname -s)
HOST := $(shell hostname)

ifeq ($(UNAME),Darwin)
	CPPFLAGS= -I../game-libs/out/include/SDL2 -D_REENTRANT
	LDFLAGS= -L../game-libs/out/lib -lSDL2 -framework OpenGL \
		-Wl,-framework,ForceFeedback \
		-lobjc \
		-Wl,-framework,Cocoa \
		-Wl,-framework,Carbon \
		-Wl,-framework,IOKit \
		-Wl,-framework,CoreAudio \
		-Wl,-framework,AudioUnit
		#-Wl,-framework,AudioToolbox

else
ifeq ($(HOST),incepchow)
	CPPFLAGS= -I../game-libs64/out/include/SDL2 -D_REENTRANT
	LDFLAGS= -L../game-libs64/out/lib -lSDL2 -ldl -lpthread -lGL
endif
	SDL_LIBS =  -lSDL -lSDLmain -lSDL
endif

all : sim

%.o : %.cpp Patterns.h Ramps.h PngRamps.h PatternMixer.h
	g++ -g -c -Wall -Werror -o $@ $< $(CPPFLAGS)

sim : sim-main.o Patterns.o Ramps.o PatternMixer.o wandering_diamonds.o
	g++ -g -Wall -Werror -o $@ $^ $(LDFLAGS)

sim1 : sim-sdl1.o Patterns.o Ramps.o PatternMixer.o wandering_diamonds.o
	g++ -g -Wall -Werror -o $@ $^ $(SDL_LIBS)

clean :
	rm *.o sim sim1
