
run:
	g++ src/chip.cpp src/chip.h src/main.cpp $(shell pkg-config --cflags --libs sdl2) -o emulator

clear:
	rm emulator
