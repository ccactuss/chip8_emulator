
run:
	g++ src/chip.cpp src/chip.h src/main.cpp -std=c++11 $(shell pkg-config --cflags --libs sdl2) -o emulator

clear:
	rm emulator
