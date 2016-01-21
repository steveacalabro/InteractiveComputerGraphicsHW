// HW2

#include "Angel.h"
#include <string.h>
#include <math.h>
using namespace std;

GLuint program;
GLuint VAO[9];
float angle;
const int NUM_CIRCILE_POINTS = 100;
const int NUM_TRIANGLE_POINTS = 3;
const int NUM_SQUARE_POINTS = 4;
float red, green, blue;
//--------------------------------------------------------------------------

template <typename T>
GLuint initArrayBufferForLaterUse(T *vertexData, int numPoints)
{	
	GLuint tempBuffer;
	glGenBuffers(1, &tempBuffer); // Create a buffer object
	// Write data into the buffer object

	int dataSize = sizeof(vertexData[0])*numPoints;
	//cout << dataSize << endl;
	//cout << sizeof(vertexData)*numPoints << endl;

	glBindBuffer(GL_ARRAY_BUFFER, tempBuffer); // bind buffer to the latest bind vertex array object.
	glBufferData(GL_ARRAY_BUFFER, dataSize, &vertexData[0], GL_STATIC_DRAW); // pass vertex data to buffer
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
}

// create Vertex Array Object
template <typename T1, typename T2>
GLuint initVAO(GLuint program, T1 *verts, T2 *colors, int numPoints)
{
	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	GLuint vertBuffer = initArrayBufferForLaterUse(verts, numPoints);
	GLuint colorBuffer = initArrayBufferForLaterUse(colors, numPoints);

	int numVertComponents = sizeof(verts[0]) / sizeof(float); // assumes all elements are floats
	int numColorComponents = sizeof(colors[0]) / sizeof(float); // assumes all elements are floats

	initAttributeVariable(program, "vPosition", vertBuffer, numVertComponents);
	initAttributeVariable(program, "aColor", colorBuffer, numColorComponents);

	// unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return vao;
}


void changeSquareVerts(vec2 *verts, float x, float y, float r) {
	verts[0] = vec2(x + r*cos(DegreesToRadians * 45), y + r*sin(DegreesToRadians * 45));
	verts[1] = vec2(x + r*cos(DegreesToRadians * 135), y + r*sin(DegreesToRadians * 135));
	verts[2] = vec2(x + r*cos(DegreesToRadians * 225), y + r*sin(DegreesToRadians * 225));
	verts[3] = vec2(x + r*cos(DegreesToRadians * 315), y + r*sin(DegreesToRadians * 315));

}

void init(void)
{
	// Load shaders and use the resulting shader program based on the OS
#ifdef __linux__ 
	program = InitShader("vshader21.glsl", "fshader21.glsl");
#elif _WIN32
	program = InitShader("..\\vshader21.glsl", "..\\fshader21.glsl");
#endif
	
	

	// Specifiy the vertices and color for geometries
	float x = 0.7;
	float y = 0.7;
	float theta = 0.0;
	float r = 0.2;

	// vertices coords
	vec2 cicleVerts[NUM_CIRCILE_POINTS];
	vec2 ellipseVerts[NUM_CIRCILE_POINTS];
	vec2 triangleVerts[NUM_TRIANGLE_POINTS];
	vec2 squareVerts[NUM_SQUARE_POINTS];

	// vertices colors
	float circleColor[NUM_CIRCILE_POINTS];
	float ellipseColor[NUM_CIRCILE_POINTS];
	vec4 triangleColor[NUM_SQUARE_POINTS];
	vec4 squareColorBlack[NUM_SQUARE_POINTS];
	vec4 squareColorWhite[NUM_SQUARE_POINTS];

	// circile and ellipse
	for (int i = 0; i < NUM_CIRCILE_POINTS; i++) {
		theta = i * 360 / (NUM_CIRCILE_POINTS);
		float rad = DegreesToRadians * theta;
		cicleVerts[i] = vec2(x + r*cos(rad), y + r*sin(rad));
		circleColor[i] = theta / 360;

		ellipseVerts[i] = vec2(-x + r*cos(rad), y + 0.6*r*sin(rad));
		ellipseColor[i] = 1.0;

	}

	// triangle
	triangleVerts[0] = vec2(r*cos(DegreesToRadians * 90), y + r*sin(DegreesToRadians * 90));
	triangleVerts[1] = vec2(r*cos(DegreesToRadians * 210), y + r*sin(DegreesToRadians * 210));
	triangleVerts[2] = vec2(r*cos(DegreesToRadians * 330), y + r*sin(DegreesToRadians * 330));
	
	triangleColor[0] = vec4(1.0, 0.0, 0.0, 1.0);
	triangleColor[1] = vec4(0.0, 1.0, 0.0, 1.0);
	triangleColor[2] = vec4(0.0, 0.0, 1.0, 1.0);

	// squares colors
	for (int i = 0; i < 4; i++) {
		squareColorBlack[i] = vec4(0.0, 0.0, 0.0, 1.0);
		squareColorWhite[i] = vec4(1.0, 1.0, 1.0, 1.0);
	}


	// initialize Vertex Array Objects

	VAO[0] = initVAO(program, cicleVerts, circleColor, NUM_CIRCILE_POINTS); // circle
	VAO[1] = initVAO(program, ellipseVerts, ellipseColor, NUM_CIRCILE_POINTS); // ellipse

	VAO[2] = initVAO(program, triangleVerts, triangleColor, NUM_TRIANGLE_POINTS); // triangle

	// squares verts
	changeSquareVerts(squareVerts, 0.1, 0.1, 0.6);
	VAO[3] = initVAO(program, squareVerts, squareColorWhite, NUM_SQUARE_POINTS);

	changeSquareVerts(squareVerts, 0.1, 0.1, 0.5);
	VAO[4] = initVAO(program, squareVerts, squareColorBlack, NUM_SQUARE_POINTS);

	changeSquareVerts(squareVerts, 0.1, 0.1, 0.4);
	VAO[5] = initVAO(program, squareVerts, squareColorWhite, NUM_SQUARE_POINTS);
	
	changeSquareVerts(squareVerts, 0.1, 0.1, 0.3);
	VAO[6] = initVAO(program, squareVerts, squareColorBlack, NUM_SQUARE_POINTS);

	changeSquareVerts(squareVerts, 0.1, 0.1, 0.2);
	VAO[7] = initVAO(program, squareVerts, squareColorWhite, NUM_SQUARE_POINTS);

	changeSquareVerts(squareVerts, 0.1, 0.1, 0.1);
	VAO[8] = initVAO(program, squareVerts, squareColorBlack, NUM_SQUARE_POINTS);
	
	glUseProgram(program);
		
	glClearColor(0.0, 0.0, 0.0, 1.0); // black background
}






//----------------------------------------------------------------------------

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);     // clear the window

	// draw the color circle
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, NUM_CIRCILE_POINTS);    

	// draw the Ellipse
	glBindVertexArray(VAO[1]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, NUM_CIRCILE_POINTS);

	// draw the triangle	
	glBindVertexArray(VAO[2]);
	glDrawArrays(GL_TRIANGLES, 0, NUM_TRIANGLE_POINTS);

	// draw squares
	for (int i = 3; i < 9; i++) {
		glBindVertexArray(VAO[i]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, NUM_SQUARE_POINTS);
	}

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

void myReshape(int w, int h) {
	glViewport(0, 0, w, h);
}

//----------------------------------------------------------------------------


void timerColor(int value) {
	// get new color or a value in [0,1]
	red = (1.0*(rand() % 256)) / 256.0;
	green = (1.0*(rand() % 256)) / 256.0;
	blue = (1.0*(rand() % 256)) / 256.0;
	// draw it + reinitialise timer
	glutPostRedisplay();
	glutTimerFunc(1000, timerColor, 0);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(500, 500);

	glutCreateWindow("ICG HW2");
	glewExperimental = GL_TRUE;
	glewInit();
	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(myReshape);

	// change color each second
	glutTimerFunc(100, timerColor, 0);

	// change the shape after five seconds
	//glutTimerFunc(5000, timerShape, 0);

	glutMainLoop();
	return 0;

}
