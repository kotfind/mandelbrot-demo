LIBS=-lGL -lSDL2
WARNS=-Wall -Wextra -Wpedantic
CFLAGS=$(WARNS) $(LIBS) -O3 -g0

all: mandelbrot.out

mandelbrot.out: main.cpp
	g++ main.cpp $(CFLAGS) -o mandelbrot.out

run: mandelbrot.out
	./mandelbrot.out
