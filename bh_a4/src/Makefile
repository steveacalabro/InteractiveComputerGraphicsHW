GCC_OPTIONS=-Wall -pedantic -I ./include -std=c++11
GL_OPTIONS=-lGLEW -lGL -lglut
OPTIONS=$(GCC_OPTIONS) $(GL_OPTIONS)

all:
	g++ -c -I ./include InitShader.cpp
	g++  main.cpp InitShader.o $(OPTIONS) -o main
run:
	./main
clean:
	rm -rf *.o main

