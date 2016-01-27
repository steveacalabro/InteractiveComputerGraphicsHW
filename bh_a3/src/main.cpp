// HW3

#include "Angel.h"
#include <string>
#include <vector>
using namespace std;


GLuint program;
GLuint ellipseVAO, circleVAO, triangleVAO;
GLuint squaresVAO[6];
vector<GLuint> randomCircleVAO;
float angle, t, dr;
float dt = 0.001;
float r = 0.2;
const int NUM_CIRCILE_POINTS = 100;
const int NUM_TRIANGLE_POINTS = 3;
const int NUM_SQUARE_POINTS = 4;
const int NUM_SQUARES = 6;

int mainWindow, subWindow1, mainWindow2;
// vertices coords
vec2 cicleVerts[NUM_CIRCILE_POINTS];
vec2 ellipseVerts[NUM_CIRCILE_POINTS];
vec2 triangleVerts[NUM_TRIANGLE_POINTS];
vec2 squareVerts[NUM_SQUARE_POINTS];

// vertices colors
vec4 circleColor[NUM_CIRCILE_POINTS];
float ellipseColor[NUM_CIRCILE_POINTS];
vec4 triangleColor[NUM_SQUARE_POINTS];
vec4 squareColorBlack[NUM_SQUARE_POINTS];
vec4 squareColorWhite[NUM_SQUARE_POINTS];


float red = 1.0, green, blue;
float randR, randG, randB;
float spinSpeed = 1.0;


enum {WHITE, BLACK, GREEN, RED, BLUE};
bool stop = false;
//--------------------------------------------------------------------------

template <typename T>
GLuint initArrayBufferForLaterUse(const T *vertexData, const int numPoints, const bool isDynamic)
{	
	GLuint vbo;
	glGenBuffers(1, &vbo); // Create a buffer object
	// Write data into the buffer object

	int dataSize = sizeof(vertexData[0])*numPoints;
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // bind buffer to the latest bind vertex array object.
	if (isDynamic) {
		glBufferData(GL_ARRAY_BUFFER, dataSize, &vertexData[0], GL_DYNAMIC_DRAW); // pass vertex data to buffer		
	}
	else {		
		glBufferData(GL_ARRAY_BUFFER, dataSize, &vertexData[0], GL_STATIC_DRAW); // pass vertex data to buffer		
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
	return vbo;
}

// Assign the buffer objects and enable the assignment
void initAttributeVariable(const GLuint &program, const string a_attribute, const GLuint buffer, const int numComponents)
{
	const char *c_str = a_attribute.c_str();
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	GLuint loc = glGetAttribLocation(program, c_str);
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, numComponents, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
}

// create Vertex Array Object
template <typename T1, typename T2>
GLuint initVAO(const GLuint program, const T1 *verts, const T2 *colors, const int numPoints, const bool isDynamic)
{
	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	GLuint vertBuffer = initArrayBufferForLaterUse(verts, numPoints, isDynamic);
	GLuint colorBuffer = initArrayBufferForLaterUse(colors, numPoints, isDynamic);

	int numVertComponents = sizeof(verts[0]) / sizeof(float); // assumes all elements are floats
	int numColorComponents = sizeof(colors[0]) / sizeof(float); // assumes all elements are floats

	initAttributeVariable(program, "vPosition", vertBuffer, numVertComponents);
	initAttributeVariable(program, "aColor", colorBuffer, numColorComponents);

	// unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return vao;
}

template <typename T>
void updateVAO(const GLuint &vao, const T *vertexData, const int numPoints, ) {
	int dataSize = sizeof(vertexData[0])*numPoints;
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferSubData(vao, 0, dataSize, &vertexData[0]);
}

void changeSquareVerts(vec2 *verts, float x, float y, float r, float angle) {
	//float angle = t*spinSpeed;
	verts[0] = vec2(x + r*cos(DegreesToRadians * 45 + angle), y + r*sin(DegreesToRadians * 45 + angle));
	verts[1] = vec2(x + r*cos(DegreesToRadians * 135 + angle), y + r*sin(DegreesToRadians * 135 + angle));
	verts[2] = vec2(x + r*cos(DegreesToRadians * 225 + angle), y + r*sin(DegreesToRadians * 225 + angle));
	verts[3] = vec2(x + r*cos(DegreesToRadians * 315 + angle), y + r*sin(DegreesToRadians * 315 + angle));

}


void drawCircle(const float x, const float y, const float r) {
	// circile verts
	for (int i = 0; i < NUM_CIRCILE_POINTS; i++) {
		float theta = i * 360 / (NUM_CIRCILE_POINTS);
		float rad = DegreesToRadians * theta;
		cicleVerts[i] = vec2(x + r*cos(rad), y + r*sin(rad));
		circleColor[i] = vec4(1.0, 0.0, 0.0, 1.0);
	}
	circleVAO = initVAO(program, cicleVerts, circleColor, NUM_CIRCILE_POINTS);
}

void drawRandomCircle(const float x, const float y) {
	// circile 
	vec2 randomCicleVerts[NUM_CIRCILE_POINTS];
	vec4 randomCircleColor[NUM_CIRCILE_POINTS];
	for (int i = 0; i < NUM_CIRCILE_POINTS; i++) {
		float theta = i * 360 / (NUM_CIRCILE_POINTS);
		float rad = DegreesToRadians * theta;
		randomCicleVerts[i] = vec2(x + r*cos(rad), y + r*sin(rad));
		randomCircleColor[i] = vec4(randR, randG, randB, 1.0);
	}
	
	randomCircleVAO.push_back(initVAO(program, randomCicleVerts, randomCircleColor, NUM_CIRCILE_POINTS));
}

void drawEllipse(const float x, const float y, const float r) {
	// create ellipse verts and colors
	for (int i = 0; i < NUM_CIRCILE_POINTS; i++) {
		float theta = i * 360 / (NUM_CIRCILE_POINTS);
		float rad = DegreesToRadians * theta;
		ellipseVerts[i] = vec2(x + r*cos(rad), y + 0.6*r*sin(rad));
		ellipseColor[i] = 1.0;
	}
	ellipseVAO = initVAO(program, ellipseVerts, ellipseColor, NUM_CIRCILE_POINTS);
}

void drawTriangle(const float x, const float y, const float r) {
	// triangle
	float angle = -t*2*spinSpeed;
	triangleVerts[0] = vec2(x + r*cos(DegreesToRadians * 90 + angle),  y + r*sin(DegreesToRadians * 90 + angle));
	triangleVerts[1] = vec2(x + r*cos(DegreesToRadians * 210 + angle), y + r*sin(DegreesToRadians * 210 + angle));
	triangleVerts[2] = vec2(x + r*cos(DegreesToRadians * 330 + angle), y + r*sin(DegreesToRadians * 330 + angle));

	for (int i = 0; i < NUM_TRIANGLE_POINTS; i++) {
		triangleColor[i] = vec4(red, green, blue, 1.0);
	}

	triangleVAO = initVAO(program, triangleVerts, triangleColor, NUM_TRIANGLE_POINTS);
}


void drawSquares(const float x, const float y, const float r) {


	bool flag = true;
	float angle = t*spinSpeed;
	for (int i = 0; i < NUM_SQUARES; i++) {
		changeSquareVerts(squareVerts, x, y, r - i*0.1, angle);
		if (flag) {
			squaresVAO[i] = initVAO(program, squareVerts, squareColorWhite, NUM_SQUARE_POINTS);
		}
		else
		{
			squaresVAO[i] = initVAO(program, squareVerts, squareColorBlack, NUM_SQUARE_POINTS);
		}
		flag = !flag;		
	}
}




void initShaderProgram(void)
{
	// Load shaders and use the resulting shader program based on the OS
#ifdef __linux__ 
	program = InitShader("./shader/vshader21.glsl", "./shader/fshader21.glsl");
#elif _WIN32
	program = InitShader("..\\..\\src\\shader\\vshader21.glsl", "..\\..\\src\\shader\\fshader21.glsl");
#endif
		
	glUseProgram(program);		
	glClearColor(0.0, 0.0, 0.0, 1.0); // black background
}





//----------------------------------------------------------------------------

void displayMainWindow(void)
{
	glClear(GL_COLOR_BUFFER_BIT);     // clear the window

	drawSquares(0.0, 0.0, 0.6);
	// display spinning squares
	for (int i = 0; i < NUM_SQUARES; i++) {
		glBindVertexArray(squaresVAO[i]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, NUM_SQUARE_POINTS);
	}
	glDeleteBuffers(6, squaresVAO);


	// display random color circles
	for (int i = 0; i < randomCircleVAO.size(); i++) {
		glBindVertexArray(randomCircleVAO[i]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, NUM_CIRCILE_POINTS);
		//glDeleteBuffers(1, &randomCircleVAO[i]);
		//randomCircleVAO.erase(randomCircleVAO.begin() + i);
	}
	
	glutSwapBuffers();

}


void displaySubWindow(void)
{

	glClear(GL_COLOR_BUFFER_BIT);     // clear the window

	// display the Ellipse
	glBindVertexArray(ellipseVAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, NUM_CIRCILE_POINTS);
	glutSwapBuffers();

}

void displayMainWindow2(void)
{

	glClear(GL_COLOR_BUFFER_BIT);     // clear the window

	// display static red circle
	drawTriangle(-0.5, 0, 0.5);
	glBindVertexArray(circleVAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, NUM_CIRCILE_POINTS);


	// display triangle
	glBindVertexArray(triangleVAO);
	glDrawArrays(GL_TRIANGLES, 0, NUM_TRIANGLE_POINTS);
	glutSwapBuffers();
	glDeleteBuffers(1, &triangleVAO);

}
//----------------------------------------------------------------------------+
void subwindowMenu(int option)
{
	switch (option)
	{
		case WHITE:
			glClearColor(1.0, 1.0, 1.0, 1.0); // white background
			break;
		case BLACK:
			glClearColor(0.0, 0.0, 0.0, 1.0); // black background
			break;
		case GREEN:
			glClearColor(0.0, 1.0, 0.0, 1.0); // green background
			break;
		case RED:
			glClearColor(1.0, 0.0, 0.0, 1.0); // red background
			break;
		case BLUE:
			glClearColor(0.0, 0.0, 1.0, 1.0); // blue background
			break;
	}

	glutPostRedisplay();
}

void squareColorMenu(int option)
{
	switch (option)
	{
		case WHITE:
			for (int i = 0; i < 4; i++) {
				squareColorWhite[i] = vec4(1.0, 1.0, 1.0, 1.0);
			}
			break;
	
		case GREEN:
			for (int i = 0; i < 4; i++) {
				squareColorWhite[i] = vec4(0.0, 1.0, 0.0, 1.0);
			}
			break;
		case RED:
			for (int i = 0; i < 4; i++) {
				squareColorWhite[i] = vec4(1.0, 0.0, 0.0, 1.0);
			}
			break;

	}

	glutPostRedisplay();
}

void animationMenu(int option)
{
	switch (option)
	{
	case 0:
		stop = true;
		break;
	case 1:
		stop = false;
		break;
	}

	glutPostRedisplay();
}

void createAnimationMenus() {
	GLuint menu, submenu;
	
	// create a sub menu and add entries to it
	
	submenu = glutCreateMenu(squareColorMenu);
	glutAddMenuEntry("White", WHITE);
	glutAddMenuEntry("Red", RED);
	glutAddMenuEntry("Green", GREEN);

	// create a parent menu
	menu = glutCreateMenu(animationMenu);
	// add entries to parent menu
	glutAddMenuEntry("Stop Animation", 0);
	glutAddMenuEntry("Start Animation", 1);
	//glutAddMenuEntry("Square Colors", 2);

	// add sub menu to parent menu
	glutAddSubMenu("Square Colors", submenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}


void createColorMenus() {
	GLuint menu_id = glutCreateMenu(subwindowMenu);
	glutAddMenuEntry("White", WHITE);
	glutAddMenuEntry("Red", RED);
	glutAddMenuEntry("Blue", BLUE);
	glutAddMenuEntry("Green", GREEN);
	glutAddMenuEntry("Black", BLACK);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void myMouse(GLint button, GLint state, GLint x, GLint y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		float normalizedX, normalizedY;
		normalizedX = 2.0f * x / 500 - 1;
		normalizedY = 2.0f * (500 - y) / 500 - 1;
		drawRandomCircle(normalizedX, normalizedY);
	}
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case 'r': red = 1.0; green = 0.0; blue = 0.0; break;
		case 'g': red = 0.0; green = 1.0; blue = 0.0; break;
		case 'b': red = 0.0; green = 0.0; blue = 1.0; break;
		case 'y': red = 1.0; green = 1.0; blue = 0.0; break;
		case 'o': red = 1.0; green = 0.5; blue = 0.0; break;
		case 'p': red = 1.0; green = 0.0; blue = 1.0; break;
		case 'w': red = 1.0; green = 1.0; blue = 1.0; break;
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


void timerColor(const int value) {
	// get new color or a value in [0,1]
	randR = (1.0*(rand() % 256)) / 256.0;
	randG = (1.0*(rand() % 256)) / 256.0;
	randB = (1.0*(rand() % 256)) / 256.0;
	// draw it + reinitialise timer
	glutPostWindowRedisplay(mainWindow);
	glutTimerFunc(100, timerColor, 0);
}

void timerShape(const int value) {
	// get new radius

	if (r > 1.0) {
		r -= dr;
	}
	else {
		r += dr;
	}
	// draw it + reinitialise timer
	glutPostRedisplay();
	glutTimerFunc(500, timerShape, 0);
}

void myIdle() {
	/* change something over time*/
	if (!stop) {
		t += dt;
	}
	
	glutPostWindowRedisplay(mainWindow);
	glutPostWindowRedisplay(mainWindow2);
}



int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	

	// main window
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(500, 500);
	mainWindow = glutCreateWindow("ICG HW3");
	glewExperimental = GL_TRUE;
	glewInit();
	initShaderProgram();
	// set squares colors

	for (int i = 0; i < 4; i++) {
		squareColorBlack[i] = vec4(0.0, 0.0, 0.0, 1.0);
		squareColorWhite[i] = vec4(1.0, 1.0, 1.0, 1.0);
	}

	glutDisplayFunc(displayMainWindow);
	glutMouseFunc(myMouse);
	createAnimationMenus();
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(myReshape);
	// change color each second
	glutTimerFunc(100, timerColor, 0);
	glutIdleFunc(myIdle);


	// change the shape after five seconds
	//glutTimerFunc(5000, timerShape, 0);



	// subwindow
	int border = 0;
	int w = 150;
	int h = 120;

	subWindow1 = glutCreateSubWindow(mainWindow, border, border, w, h );
	createColorMenus();
	initShaderProgram();
	drawEllipse(0.0, 0.0, 0.5);

	
	// Must register a display func for each window
	glutDisplayFunc(displaySubWindow);



	// windows 2 that displays triangle and circle
	glutInitWindowPosition(600, 100);
	glutInitWindowSize(500, 500);


	// main window 2
	mainWindow2 = glutCreateWindow("window 2");
	initShaderProgram();
	drawCircle(0.5, 0.0, 0.5);
	glutDisplayFunc(displayMainWindow2);
	//glutMouseFunc(myMouse);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(myReshape);
	glutIdleFunc(myIdle);

	glutMainLoop();



	return 0;

}
