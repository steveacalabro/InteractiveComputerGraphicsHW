GCC_OPTIONS=-Wall -pedantic -I include
GL_OPTIONS=-lGLEW -lGL -lglut
OPTIONS=$(GCC_OPTIONS) $(GL_OPTIONS)

all:
	g++  main.cpp Common/InitShader.o $(OPTIONS) -o main
run:
	./main
clean:
	rm -rf *.o main

