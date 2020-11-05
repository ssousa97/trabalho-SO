CC=gcc
MAIN=scheduler.o
DEPENDENCIES=cpu.o queue.o process.o
BIN=scheduler

all: build

build: scheduler cpu queue process
	$(CC) $(MAIN) $(DEPENDENCIES) -o $(BIN)

scheduler:
	$(CC) -c scheduler.c

cpu:
	$(CC) -c cpu.c

queue:
	$(CC) -c queue.c

process:
	$(CC) -c process.c

run: build
	./$(BIN)

clean:
	rm $(BIN) $(MAIN) $(DEPENDENCIES)