GCC_OPTIONS=-Wall -pedantic -I include
GL_OPTIONS=-lGLEW -lGL -lglut
OPTIONS=$(GCC_OPTIONS) $(GL_OPTIONS)

all:
	g++  main.cpp Common/InitShader.o $(OPTIONS) -o main
	g++  part2.cpp Common/InitShader.o $(OPTIONS) -o part2
run1:
	./main
run2:
	./part2
clean:
	rm -rf *.o main part2

