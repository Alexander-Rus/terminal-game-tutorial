CFLAGS = -std=c++11 -Wall -pedantic

all: bin/main

bin/main: bin/main.o bin/game.o bin/ObjectField.o bin/Player.o bin/WindowManager.o
	g++ -g -o bin/main bin/main.o bin/game.o bin/ObjectField.o bin/Player.o bin/WindowManager.o $(CFLAGS) -lncurses

bin/main.o: src/main.cpp
	mkdir -p bin	
	g++ -g -c -o bin/main.o src/main.cpp $(CFLAGS) 

bin/game.o: src/game.cpp src/game.h src/ObjectField.h
	mkdir -p bin	
	g++ -g -c -o bin/game.o src/game.cpp $(CFLAGS)

bin/ObjectField.o: src/ObjectField.cpp src/ObjectField.h
	mkdir -p bin	
	g++ -g -c -o bin/ObjectField.o src/ObjectField.cpp $(CFLAGS)
	
bin/Player.o: src/Player.cpp src/Player.h
	mkdir -p bin	
	g++ -g -c -o bin/Player.o src/Player.cpp $(CFLAGS)
	
bin/WindowManager.o: src/WindowManager.cpp src/WindowManager.h
	mkdir -p bin	
	g++ -g -c -o bin/WindowManager.o src/WindowManager.cpp $(CFLAGS)
