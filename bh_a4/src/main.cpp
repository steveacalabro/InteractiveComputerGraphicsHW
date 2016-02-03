// HW4
#include "main.h"
#include "BoxGeometry.h"
using namespace std;


//-------------------- Gloabal variables ----------------------------------------------
GLenum err;

// global shader program
GLuint program;

// global vertex array object
GLuint vao;

// main window id
int mainWindow;

enum class VBO_OPTION{ VERTEX, COLOR };

// for right click menu
bool onScale{ false }, onRotate{ false }, onTranslate{ true };

// mode, camera and scene class
BoxGeometry ColorCube;
Camera camera;
Scene scene;



const int NumVertices = 36;
color4 colors[NumVertices];
point4 points[NumVertices];

// quad generates two triangles for each face and assigns colors
// to the vertices
int Index = 0;

void quad(int a, int b, int c, int d)
{
	// triangle 1
	colors[Index] = ColorCube.vertex_colors[a]; points[Index] = ColorCube.vertices[a]; Index++;
	colors[Index] = ColorCube.vertex_colors[b]; points[Index] = ColorCube.vertices[b]; Index++;
	colors[Index] = ColorCube.vertex_colors[c]; points[Index] = ColorCube.vertices[c]; Index++;

	// triangle 2
	colors[Index] = ColorCube.vertex_colors[a]; points[Index] = ColorCube.vertices[a]; Index++;
	colors[Index] = ColorCube.vertex_colors[c]; points[Index] = ColorCube.vertices[c]; Index++;
	colors[Index] = ColorCube.vertex_colors[d]; points[Index] = ColorCube.vertices[d]; Index++;
}

// generate 12 triangles: 36 vertices and 36 colors
void colorCube()
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}


void initColorCube()
{
	colorCube();
	// Create a vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) +
		sizeof(colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0,
		sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points),
		sizeof(colors), colors);

	glUseProgram(program);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));
	GLuint aColor = glGetAttribLocation(program, "aColor");
	glEnableVertexAttribArray(aColor);
	glVertexAttribPointer(aColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//thetaLoc = glGetUniformLocation(program, "theta");
}



template <typename T>
GLuint initVBO(const T *vertexData, const int numPoints, const bool isDynamic)
{
	GLuint vbo;
	glGenBuffers(1, &vbo); // Create a buffer object
	// Write data into the buffer object

	int dataSize = sizeof(vertexData[0])*numPoints;
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // bind buffer to the latest bind vertex array object.
	if (isDynamic) {
		glBufferData(GL_ARRAY_BUFFER, dataSize, &vertexData[0], GL_STREAM_DRAW); // pass vertex data to buffer		
	}
	else {
		glBufferData(GL_ARRAY_BUFFER, dataSize, &vertexData[0], GL_STATIC_DRAW); // pass vertex data to buffer		
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
	return vbo;
}

// create an element buffer object 
template <typename T>
GLuint initEBO(const T *indices, const int numElements, const bool isDynamic)
{

	GLuint ebo;
	glGenBuffers(1, &ebo);
	if (!ebo)
	{
		cout << "Failed to create the index buffer object!\n";
		return NULL;
	}


	// Write date into the buffer object
	int dataSize = sizeof(indices[0])*numElements;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	if (isDynamic) {
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize, &indices[0], GL_STREAM_DRAW); // pass elements data to buffer		
	}
	else {
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize, &indices[0], GL_STATIC_DRAW); // pass elements data to buffer		
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);

	return ebo;
}


// Assign the buffer objects and enable the assignment
void initAttributeVariable(const GLuint &program, const string a_attribute, const GLuint buffer, const int numComponents)
{
	const char *c_str = a_attribute.c_str();
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	GLuint loc = glGetAttribLocation(program, c_str);
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, numComponents, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(nullptr));
}

// create Vertex Array Object
template <typename T1, typename T2>
VertexArrayObject initVAO(const GLuint program, const T1 *verts, const T2 *indices, const int numPoints, const int numElements, const bool isDynamic)
{
	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);


	// init vertex and color vbo
	GLuint vertBuffer = initVBO(verts, numPoints, isDynamic);
	GLuint elementBuffer = initEBO(indices, numElements, isDynamic);

	int numVertComponents = sizeof(verts[0]) / sizeof(float); // assumes all elements are floats
	numVertComponents = 3; // no matter we use vec3 or array, one vertex has 3 components

	initAttributeVariable(program, "vPosition", vertBuffer, numVertComponents);


	// unbind vao
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VertexArrayObject{ vao, vertBuffer, elementBuffer };
}

// update the vbo that is binded by the vao
template <typename T>
void updateVAO(VertexArrayObject &VAO, const T *vertData, const int numPoints, const int whichVBO) {
	int dataSize = sizeof(vertData[0])*numPoints;

	GLuint vbo = -1;
	if (whichVBO == VBO_OPTION::COLOR) {
		//vbo = VAO.colorVBO;
	}
	else if (whichVBO == VBO_OPTION::VERTEX) {
		vbo = VAO.vertVBO;
	}
	else {
		cout << "No such VBO in this VAO\n";
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, &vertData[0]);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
}




void initShaderProgram(void)
{
	// Load shaders and use the resulting shader program based on the OS
#ifdef __linux__ 
	program = InitShader("./shader/vshader21.glsl", "./shader/fshader21.glsl");
#elif _WIN32
	program = InitShader("..\\src\\shader\\vshader21.glsl", "..\\src\\shader\\fshader21.glsl");
#endif

	glUseProgram(program);
	glClearColor(0.0, 0.0, 0.0, 1.0); // black background
}





//----------------------------------------------------------------------------
void render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera.updateMatrix();
	ColorCube.updateMatrix();
	scene.updateMatrix();

	mat4 MVP = camera.projViewMatrix * scene.compositeMatrix * ColorCube.compositeMatrix;

	// send model view projection maatrix to shader
	GLuint u_MVP = glGetUniformLocation(program, "u_MVP");
	glUniformMatrix4fv(u_MVP, 1, GL_TRUE, MVP); // mat.h is row major, so use GL_TRUE to transpsoe t

	// bind vertex array object
	//glBindVertexArray(ColorCube.vao.id);

	/*glBindBuffer(GL_VERTEX_ARRAY, ColorCube.vao.vertVBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ColorCube.vao.EBO);
*/
//glDrawElements(GL_TRIANGLES, ColorCube.NUM_FACES*3, GL_UNSIGNED_INT, 0); // Draw the cube's faces
//glDrawArrays(GL_TRIANGLES, 0, ColorCube.NUM_VERTS*3); // Draw the cube's faces


	glBindBuffer(GL_VERTEX_ARRAY, vao);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}

void displayMainWindow(void)
{
	//glClear(GL_COLOR_BUFFER_BIT);     // clear the window
	render();

	glutSwapBuffers();

}



void animationMenu(int option)
{
	switch (option)
	{
	case 0:
		onScale = true;
		onRotate = false;
		onTranslate = false;
		break;
	case 1:
		onScale = false;
		onRotate = true;
		onTranslate = false;
		break;

	case 2:
		onScale = false;
		onRotate = false;
		onTranslate = true;
		break;
	}

	glutPostRedisplay();
}

void createAnimationMenus() {
	GLuint menu;

	// create a parent menu
	menu = glutCreateMenu(animationMenu);
	// add entries to parent menu
	glutAddMenuEntry("Scale", 0);
	glutAddMenuEntry("Rotate", 1);
	glutAddMenuEntry("Translate", 2);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void resetTransformation() {
	ColorCube.setIdentity();
}


void myKeyboard(unsigned char key, int x, int y)
{
	auto TX{ 0.0 }, TY{ 0.0 }, TZ{ 0.0 };
	static auto delta{ 0.5 };
	switch (key) {
		// translate X
	case 'd':case 'D': TX = delta; break;
	case 'a':case 'A': TX = -delta; break;

		// translate Y
	case 'w':case 'W': TY = delta; break;
	case 's':case 'S': TY = -delta; break;

		// translate Z		
	case 'e': case 'E': TZ = delta; break;
	case 'q': case 'Q': TZ = -delta; break;

		// reset transformations
	case 'R': case 'r': resetTransformation(); fprintf(stdout, "Transformations reseted\n"); break;

		// decrease delta
	case 'z': case 'Z':
		if (delta > 0.0)
		{
			delta -= 0.01;
		}
		else if (delta < 0.0)
		{
			delta = 0.0;
			fprintf(stdout, "Press 'X' to increase delta!\n");
		}

		fprintf(stdout, "delta = %.2f\n", delta);
		break;

		// increase delta
	case 'x': case 'X': delta += 0.01;
		if (delta > 0.9)
		{
			delta = 0.9;
			fprintf(stdout, "Press 'Z' to decrease delta!\n");
		}
		fprintf(stdout, "delta = %.2f\n", delta); break;

		// exit program
	case 033:
		glDeleteProgram(program);
		glutDestroyWindow(mainWindow);
		exit(EXIT_SUCCESS);
	}

	if (TX != 0.0 || TY != 0.0 || TZ != 0.0)
	{
		if (onScale) {
			double sx{ 1 + TX }, sy{ 1 + TY }, sz{ 1 + TZ };
			ColorCube.scale(sx, sy, sz);
			fprintf(stdout, "scale color cube by : (%.2f, %.2f, %.2f)\n", sx, sy, sz);
		}
		else if (onRotate) {
			ColorCube.rotate(TX, TY, TZ);
			fprintf(stdout, "rotate color cube by : (%.2f, %.2f, %.2f)\n", TX, TY, TZ);
		}
		else if (onTranslate) {
			ColorCube.translate(TX, TY, TZ);
			fprintf(stdout, "translate color cube by : (%.2f, %.2f, %.2f)\n", TX, TY, TZ);
			//ColorCube.translationMatrix[0][3] += TX;
			//ColorCube.translationMatrix[1][3] += TY;
			//ColorCube.translationMatrix[2][3] += TZ;
		}
	}


	glutPostRedisplay();
}

//----------------------------------------------------------------------------

void myReshape(int w, int h) {
	glViewport(0, 0, w, h);
}

//----------------------------------------------------------------------------



void assignGlutFunctions(void(*display)(), void(*keyboad)(unsigned char, int, int), void(*mouse)(int, int, int, int),
	void(*reshape)(int, int), void(*timer)(int), void(*idle)(), void(*menuFunc)())
{

	glutDisplayFunc(display);

	if (mouse) {
		glutMouseFunc(mouse);
	}

	if (keyboad) {
		glutKeyboardFunc(keyboad);
	}

	if (reshape) {
		glutReshapeFunc(reshape);
	}

	if (timer) {
		int interval = 100;
		glutTimerFunc(interval, timer, 0);
	}
	if (idle) {
		glutIdleFunc(idle);
	}


	// menu function
	if (menuFunc) {
		(*menuFunc)();
	}

}



int createOpenGLContext(const char* windowName, const int x, const int y, const int w, const int h) {

	glutInitWindowPosition(x, y);
	glutInitWindowSize(w, h);

	int windowID = glutCreateWindow(windowName);

	if (!err) {
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
	}

	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		//fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	//fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	initShaderProgram(); // note that each windows should have a shader program
	return windowID;
}

void initCamera(Camera &camera, bool usePerspective)
{
	// init camera view matrix
	vec3 cameraPosition = vec3(0.0, 0.0, 10.0);
	vec3 cameraTarget = vec3(0.0, 0.0, 0.0);
	vec3 cameraUp = vec3(0.0, 1.0, 0.0);
	camera.LookAt(cameraPosition, cameraTarget, cameraUp);

	// init camera projection matrix	
	if (usePerspective)
	{
		// perspective projection
		double fovy{ 50.0 }, aspect{ 1.0 }, zNear{ 1.0 }, zFar{ 10.0 };
		camera.Perspective(fovy, aspect, zNear, zFar);
	}
	else
	{
		// orthographic projection
		double left{ -2.0 }, right{ 2.0 }, bottom{ -2.0 }, top{ 2.0 }, zNear{ 0.001 }, zFar{ 100.0 };
		camera.Ortho(left, right, bottom, top, zNear, zFar);

	}
}

void printInstructions()
{
	fprintf(stdout, "***************************** Instructions *********************************** \n");
	fprintf(stdout, "1. Use 'A', 'D', 'W', 'S', 'Q', 'E' for increasing and decreasing the X,Y,Z components of the current transformation \n");
	fprintf(stdout, "2. Use 'Z', 'X' for increasing and decreasing the delta added to the transformations \n");
	fprintf(stdout, "3. Press 'R' to reset the transformations \n");

}
void initScene() {
	printInstructions();

	initCamera(camera, false);
	// init color cube
	initColorCube();
	//ColorCube.init(program);
}



//------------------------ Main function ------------------------------------------
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);

	///********* main window  ****************/ 
	mainWindow = createOpenGLContext("ICG HW4 - Color Cube", 200, 0, 500, 500);
	assignGlutFunctions(displayMainWindow, myKeyboard, nullptr, nullptr, nullptr, nullptr, createAnimationMenus);
	initScene();

	glutMainLoop();
	int k;
	cin >> k;
	return 0;

}
