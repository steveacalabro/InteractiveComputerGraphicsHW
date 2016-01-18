// First new GL program
// Just makes a red triangle

#include "Angel.h"
#include <string.h>
using namespace std;
const int NumPoints = 3;
GLuint buffer, buffer1, buffer2;
GLuint program;
//--------------------------------------------------------------------------


GLuint initArrayBufferForLaterUse(vec2 vertexData[], int numPoints)
{	
	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint tempBuffer;
	glGenBuffers(1, &tempBuffer); // Create a buffer object
	// Write data into the buffer object
	glBindBuffer(GL_ARRAY_BUFFER, tempBuffer); // bind buffer to the latest bind vertex array object.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData)*numPoints, &vertexData[0], GL_STATIC_DRAW); // pass vertex data to buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);

	return tempBuffer;
}

// Assign the buffer objects and enable the assignment
void initAttributeVariable(GLuint &program, string a_attribute, GLuint buffer, int numComponents)
{
	const char *c_str = a_attribute.c_str();
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	GLuint loc = glGetAttribLocation(program, c_str);
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, numComponents, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	//glBindBuffer(GL_ARRAY_BUFFER, NULL);
}



void init(void)
{
	// Specifiy the vertices for a triangle
	vec2 vertices[3] = {
		vec2(-0.85, -0.715), vec2(-0.15, -0.5615), vec2(-0.05, -0.255)
	};

	vec2 vertices1[4] = {
		vec2(0.25, -0.135), vec2(0.25, 0.135), vec2(0.755, -0.135),vec2(0.755, 0.135)
	};

	vec2 vertices2[5] = {
		vec2(-0.85, 0.25), vec2(-0.15, 0.9815), vec2(0.034, -0.0835),vec2(0.045, 0.705), vec2(0.45, 0.512)
	};

	// initialize buffers
	buffer = initArrayBufferForLaterUse(vertices,3);
	buffer1 = initArrayBufferForLaterUse(vertices1,4);
	buffer2 = initArrayBufferForLaterUse(vertices2,5);

	cout<<(sizeof(vertices) / sizeof(vertices[0]));
	// Load shaders and use the resulting shader program
	program = InitShader("..\\vshader21.glsl", "..\\fshader21.glsl");
	glUseProgram(program);

	// Initialize the vertex position attribute from the vertex shader

	glClearColor(1.0, 1.0, 1.0, 1.0); // white background
}






//----------------------------------------------------------------------------

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);     // clear the window

	initAttributeVariable(program, "vPosition", buffer, 2);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    // draw the triangle	


	initAttributeVariable(program, "vPosition", buffer1, 2);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);    // draw the quadrilateral 


	initAttributeVariable(program, "vPosition", buffer2, 2);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);    // draw the pentagon

	glFlush();
}

//----------------------------------------------------------------------------

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033:
		exit(EXIT_SUCCESS);
		break;
	}
}

//----------------------------------------------------------------------------

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(512, 512);

	glutCreateWindow("Red Triangle");
	glewExperimental = GL_TRUE;
	glewInit();
	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);

	glutMainLoop();
	return 0;

}
