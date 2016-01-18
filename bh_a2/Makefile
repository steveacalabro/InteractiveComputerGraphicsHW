GCC_OPTIONS=-Wall -pedantic -I include
GL_OPTIONS=-lGLEW -lGL -lglut
OPTIONS=$(GCC_OPTIONS) $(GL_OPTIONS)

all:
	g++  red_triangle.cpp Common/InitShader.o $(OPTIONS) -o red_triangle
run:
	./red_triangle
clean:
	rm -rf *.o red_triangle

