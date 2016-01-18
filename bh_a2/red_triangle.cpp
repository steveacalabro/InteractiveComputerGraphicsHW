// First new GL program
// Just makes a red triangle

#include "Angel.h"
#include <string.h>
using namespace std;
const int NumPoints = 3;
GLuint triangleBuffer, triangleColorBuffer, ellipseBuffer, ellipseColorBuffer, circleBuffer, circleColorBuffer, 
squareBuffer0, squareBuffer1, squareBuffer2, squareBuffer3, squareBuffer4, squareBuffer5, squareColorBufferBlack, squareColorBufferWhite;
GLuint program;
float angle;
vec2 squareVerts[4];
const int NUM_CIRCILE_POINTS = 100;
GLuint VBO, VAO;
//--------------------------------------------------------------------------

template <typename T>
GLuint initArrayBufferForLaterUse(T vertexData[], int numPoints)
{	
	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint tempBuffer;
	glGenBuffers(1, &tempBuffer); // Create a buffer object
	// Write data into the buffer object

	int dataSize = sizeof(vertexData[0])*numPoints;
	cout << dataSize << endl;
	cout << sizeof(vertexData)*numPoints << endl;

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

	//glBindBuffer(GL_ARRAY_BUFFER, NULL);
}

void changeSquareVerts(vec2 *verts, float x, float y, float r) {
	//vec2 verts[4];
	verts[0] = vec2(x+r*cos(DegreesToRadians * 45), y + r*sin(DegreesToRadians * 45));
	verts[1] = vec2(x+r*cos(DegreesToRadians * 135), y + r*sin(DegreesToRadians * 135));
	verts[2] = vec2(x+r*cos(DegreesToRadians * 225), y + r*sin(DegreesToRadians * 225));
	verts[3] = vec2(x+r*cos(DegreesToRadians * 315), y + r*sin(DegreesToRadians * 315));

}

void init(void)
{
	
	// Specifiy the vertices and color for geometries
	float x = 0.7;
	float y = 0.7;
	float theta = 0.0;
	float r = 0.2;

	vec2 cicleVerts[NUM_CIRCILE_POINTS];
	vec2 ellipseVerts[NUM_CIRCILE_POINTS];
	vec2 triangleVerts[3];
	vec4 triangleColor[3];
	vec4 squareColorBlack[4];
	vec4 squareColorWhite[4];
	float circleColor[NUM_CIRCILE_POINTS];
	float ellipseColor[NUM_CIRCILE_POINTS];
	//vec4 ellipseColor[NUM_CIRCILE_POINTS];

	triangleVerts[0] = vec2(r*cos(DegreesToRadians * 90), y + r*sin(DegreesToRadians * 90));
	triangleVerts[1] = vec2(r*cos(DegreesToRadians * 210), y + r*sin(DegreesToRadians * 210));
	triangleVerts[2] = vec2(r*cos(DegreesToRadians * 330), y + r*sin(DegreesToRadians * 330));

	
	triangleColor[0] = vec4(1.0, 0.0, 0.0, 1.0);
	triangleColor[1] = vec4(0.0, 1.0, 0.0, 1.0);
	triangleColor[2] = vec4(0.0, 0.0, 1.0, 1.0);

	
	for (int i = 0; i < 4; i++) {
		squareColorBlack[i] = vec4(0.0, 0.0, 0.0, 1.0);
		squareColorWhite[i] = vec4(1.0, 1.0, 1.0, 1.0);

		/*squareColorBlack[0] = vec4(0.0, 0.0, 0.0, 0.0);
		squareColorBlack[1] = vec4(0.0, 0.0, 0.0, 0.0);
		squareColorBlack[2] = vec4(0.0, 0.0, 0.0, 0.0);
		squareColorBlack[3] = vec4(0.0, 0.0, 0.0, 0.0);

		squareColorWhite[0] = vec4(1.0, 1.0, 1.0, 1.0);
		squareColorWhite[1] = vec4(1.0, 1.0, 1.0, 1.0);
		squareColorWhite[2] = vec4(1.0, 1.0, 1.0, 1.0);
		squareColorWhite[3] = vec4(1.0, 1.0, 1.0, 1.0);*/
	}



	for (int i = 0; i < NUM_CIRCILE_POINTS; i++) {
		theta = i * 360 / (NUM_CIRCILE_POINTS);
		float rad = DegreesToRadians*theta;
		cicleVerts[i] = vec2(x + r*cos(rad), y + r*sin(rad));
		circleColor[i] = theta/360;

		ellipseVerts[i] = vec2(-x + r*cos(rad), y + 0.6*r*sin(rad));
		ellipseColor[i] = 1.0;
		
	}

	// initialize buffers
	ellipseBuffer = initArrayBufferForLaterUse(ellipseVerts, NUM_CIRCILE_POINTS);
	ellipseColorBuffer = initArrayBufferForLaterUse(ellipseColor, NUM_CIRCILE_POINTS);

	circleBuffer = initArrayBufferForLaterUse(cicleVerts, NUM_CIRCILE_POINTS);
	circleColorBuffer = initArrayBufferForLaterUse(circleColor, NUM_CIRCILE_POINTS);

	triangleBuffer = initArrayBufferForLaterUse(triangleVerts, 3);
	triangleColorBuffer = initArrayBufferForLaterUse(triangleColor, 3);

	squareColorBufferBlack = initArrayBufferForLaterUse(squareColorBlack, 4);
	squareColorBufferWhite = initArrayBufferForLaterUse(squareColorWhite, 4);

	changeSquareVerts(squareVerts, 0.1, 0.1, 0.5);
	squareBuffer0 = initArrayBufferForLaterUse(squareVerts, 4);

	changeSquareVerts(squareVerts, 0.1, 0.1, 0.4);
	squareBuffer1 = initArrayBufferForLaterUse(squareVerts, 4);
	
	changeSquareVerts(squareVerts, 0.1, 0.1, 0.3);
	squareBuffer2 = initArrayBufferForLaterUse(squareVerts, 4);

	changeSquareVerts(squareVerts, 0.1, 0.1, 0.2);
	squareBuffer3 = initArrayBufferForLaterUse(squareVerts, 4);

	changeSquareVerts(squareVerts, 0.1, 0.1, 0.1);
	squareBuffer4 = initArrayBufferForLaterUse(squareVerts, 4);

	changeSquareVerts(squareVerts, 0.1, 0.1, 0.05);
	squareBuffer5 = initArrayBufferForLaterUse(squareVerts, 4);

	// Load shaders and use the resulting shader program
	program = InitShader("..\\vshader21.glsl", "..\\fshader21.glsl");
	glUseProgram(program);

	// Initialize the vertex position attribute from the vertex shader
	glClearColor(0.0, 0.0, 0.0, 1.0); // black background
}






//----------------------------------------------------------------------------

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);     // clear the window

	// draw the Ellipse
	initAttributeVariable(program, "vPosition", ellipseBuffer, 2);
	initAttributeVariable(program, "aColor", ellipseColorBuffer, 1);
	glDrawArrays(GL_TRIANGLE_FAN, 0, NUM_CIRCILE_POINTS);    


	// draw the color circle
	initAttributeVariable(program, "vPosition", circleBuffer, 2);
	initAttributeVariable(program, "aColor", circleColorBuffer, 1);
	glDrawArrays(GL_TRIANGLE_FAN, 0, NUM_CIRCILE_POINTS);    

	// draw the triangle	
	initAttributeVariable(program, "vPosition", triangleBuffer, 2);
	initAttributeVariable(program, "aColor", triangleColorBuffer, 4);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// draw the biggest white square	
	initAttributeVariable(program, "vPosition", squareBuffer0, 2);
	initAttributeVariable(program, "aColor", squareColorBufferWhite, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	// draw the biggest black square	
	initAttributeVariable(program, "vPosition", squareBuffer1, 2);
	initAttributeVariable(program, "aColor", squareColorBufferBlack, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	// draw the smaller white square	
	initAttributeVariable(program, "vPosition", squareBuffer2, 2);
	initAttributeVariable(program, "aColor", squareColorBufferWhite, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	// draw the smaller white square	
	initAttributeVariable(program, "vPosition", squareBuffer3, 2);
	initAttributeVariable(program, "aColor", squareColorBufferBlack, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	// draw the smaller white square	
	initAttributeVariable(program, "vPosition", squareBuffer4, 2);
	initAttributeVariable(program, "aColor", squareColorBufferWhite, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	// draw the smaller white square	
	initAttributeVariable(program, "vPosition", squareBuffer5, 2);
	initAttributeVariable(program, "aColor", squareColorBufferBlack, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);


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
