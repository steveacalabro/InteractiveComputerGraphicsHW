GCC_OPTIONS=-Wall -pedantic -I ./include -std=c++11
GL_OPTIONS=-lGLEW -lGL -lglut
OPTIONS=$(GCC_OPTIONS) $(GL_OPTIONS)

all:
	g++ -c -I ./include InitShader.cpp
	g++ -c -I ./include Plane.cpp $(OPTIONS)
	g++ -c -I ./include loadSMF.cpp $(OPTIONS)
	g++  main.cpp InitShader.o Plane.o loadSMF.o $(OPTIONS) -o main
run:
	./main
clean:
	rm -rf *.o main

