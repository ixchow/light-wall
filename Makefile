.PHONY : all

UNAME := $(shell uname -s)

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
	CPPFLAGS= -I../game-libs64/out/include/SDL2 -D_REENTRANT
	LDFLAGS= -L../game-libs64/out/lib -lSDL2 -ldl -lpthread -lGL
endif

all : sim

%.o : %.cpp Patterns.h Ramps.h
	g++ -c -Wall -Werror -o $@ $< $(CPPFLAGS)

sim : sim-main.o Patterns.o Ramps.o PatternMixer.o
	g++ -Wall -Werror -o $@ $^ $(LDFLAGS)
