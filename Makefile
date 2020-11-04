all: build

build: trabalho.c
	gcc trabalho.c -o trabalho

run: build
	./trabalho

clean:
	rm trabalho