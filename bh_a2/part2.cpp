// HW2

#include "Angel.h"
#include <string.h>
using namespace std;

GLuint program;
GLuint VAO[9];
float angle;
const int NUM_CIRCILE_POINTS = 50;
const int NUM_TRIANGLE_POINTS = 4;
const int NUM_SQUARE_POINTS = 5;

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
	verts[1] = vec2(x + r*cos(DegreesToRadians * 90), y + r*sin(DegreesToRadians * 90));
	verts[2] = vec2(x + r*cos(DegreesToRadians * 180), y + r*sin(DegreesToRadians * 180));
	verts[3] = vec2(x + r*cos(DegreesToRadians * 270), y + r*sin(DegreesToRadians * 270));
	verts[4] = vec2(x + r*cos(DegreesToRadians * 330), y + r*sin(DegreesToRadians * 330));

}

void init(void)
{

#ifdef __linux__ 
	program = InitShader("vshader21.glsl", "fshader21.glsl");
#elif _WIN32
	program = InitShader("..\\vshader21.glsl", "..\\fshader21.glsl");
#endif
	// Load shaders and use the resulting shader program
	

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
	vec3 circleColor[NUM_CIRCILE_POINTS];
	vec3 ellipseColor[NUM_CIRCILE_POINTS];
	vec4 triangleColor[NUM_SQUARE_POINTS];
	vec4 squareColorBlack[NUM_SQUARE_POINTS];
	vec4 squareColorWhite[NUM_SQUARE_POINTS];

	// circile and ellipse
	for (int i = 0; i < NUM_CIRCILE_POINTS; i++) {
		theta = i * 360 / (NUM_CIRCILE_POINTS);
		float rad = DegreesToRadians * theta;
		cicleVerts[i] = vec2(x + r*cos(rad), y + r*sin(rad));
		circleColor[i] = vec3(theta / 360, theta / 720, theta / 540);

		ellipseVerts[i] = vec2(-x + 0.4*r*cos(rad), y + 0.6*r*sin(rad));
		ellipseColor[i] = vec3(theta / 720, theta / 400, theta / 360);

	}

	// triangle
	r = r+0.3;
	triangleVerts[0] = vec2(r*cos(DegreesToRadians * 90), y + r*sin(DegreesToRadians * 90));
	triangleVerts[1] = vec2(r*cos(DegreesToRadians * 150), y + r*sin(DegreesToRadians * 150));
	triangleVerts[2] = vec2(r*cos(DegreesToRadians * 210), y + r*sin(DegreesToRadians * 210));
	triangleVerts[3] = vec2(r*cos(DegreesToRadians * 315), y + r*sin(DegreesToRadians * 315));
	
	triangleColor[0] = vec4(1.0, 0.2, 0.0, 1.0);
	triangleColor[1] = vec4(0.4, 1.0, 0.2, 1.0);
	triangleColor[2] = vec4(0.8, 0.5, 1.0, 1.0);
	triangleColor[3] = vec4(0.2, 0.6, 1.0, 1.0);

	// squares colors
	for (int i = 0; i < 4; i++) {
		squareColorBlack[i] = vec4(0.3, 0.1, 0.8, 1.0);
		squareColorWhite[i] = vec4(1.0, 0.4, 0.2, 1.0);
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

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(500, 500);

	glutCreateWindow("ICG HW2 part2");
	glewExperimental = GL_TRUE;
	glewInit();
	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);

	glutMainLoop();
	return 0;

}
