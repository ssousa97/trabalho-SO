CC=gcc
FLAGS=
MAIN=scheduler.o
DEPENDENCIES=cpu.o queue.o process.o
BIN=scheduler

all: build

build: scheduler cpu queue process
	$(CC) $(MAIN) $(DEPENDENCIES) -o $(BIN) $(FLAGS)

debug: FLAGS=-g
debug: clean build

scheduler:
	$(CC) -c scheduler.c $(FLAGS)

cpu:
	$(CC) -c cpu.c $(FLAGS)

queue:
	$(CC) -c queue.c $(FLAGS)

process:
	$(CC) -c process.c $(FLAGS)

run: build
	./$(BIN)

clean:
	rm $(BIN) *.o