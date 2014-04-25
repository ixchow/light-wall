.PHONY : all clean

LDFLAGS=-lpng

all : pngs_to_ramps

%.o : %.cpp load_save_png.hpp
	g++ -c -Wall -Werror -o $@ $< $(CPPFLAGS)

pngs_to_ramps : load_save_png.o pngs_to_ramps.o
	g++ -Wall -Werror -o $@ $^ $(LDFLAGS)

clean :
	rm *.o sim
