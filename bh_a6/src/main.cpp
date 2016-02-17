// HW6
#include "main.h"
#include "BoxGeometry.h"
#include "MeshObject.h"
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
bool onPerspective{ true };

// mode, camera and scene class
BoxGeometry ColorCube;
Camera camera;
Scene scene;
Mesh mesh;

//const int NUM_FACES{1000};


const int NumVertices = 36;
color4 colors[NumVertices];
point4 points[NumVertices];


double camRadius{ 6.0 }, camY{ 0.0 };
double speed{ 0.1 };
double t{ 0.0 }, dt{ 0.01 };
//double angle{ 0.0 };
bool stopAnimation = false;

enum {PERSPECTIVE, ORTHOGRAPHIC, PARALLEL};
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
void updateVAO(VertexArrayObject &VAO, const T *vertData, const int numPoints, VBO_OPTION whichVBO) {
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
	
	double angle = t * speed;
	double rotationCenterX = 0.0;
	double rotationCenterZ = 0.0;

	double camX = rotationCenterX + camRadius*cos(DegreesToRadians * 90 + angle);
	double camZ = rotationCenterZ + camRadius*sin(DegreesToRadians * 90 + angle);
	vec4 camPos = vec4(camX, camY, camZ, 1.0);
	vec4 camTarget = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 camUp = vec4(0.0, 1.0, 0.0, 0.0);

	camera.LookAt(camPos, camTarget, camUp);


	// update MVP
	camera.updateMatrix();
	ColorCube.updateMatrix();
	scene.updateMatrix();

	mat4 MVP = camera.projViewMatrix * scene.compositeMatrix * ColorCube.compositeMatrix;

	// send model view projection matrix to shader
	GLuint u_MVP = glGetUniformLocation(program, "u_MVP");
	glUniformMatrix4fv(u_MVP, 1, GL_TRUE, MVP); // mat.h is row major, so use GL_TRUE to transpsoe t

	// sned model view matrix to shader
	mat4 MV = camera.viewMatrix * scene.compositeMatrix * ColorCube.compositeMatrix;
	GLuint u_MV = glGetUniformLocation(program, "u_MV");
	glUniformMatrix4fv(u_MV, 1, GL_TRUE, MV); // mat.h is row major, so use GL_TRUE to transpsoe t

	// bind vertex array object
	//glBindVertexArray(ColorCube.vao.id);

	/*glBindBuffer(GL_VERTEX_ARRAY, ColorCube.vao.vertVBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ColorCube.vao.EBO);
*/
//glDrawElements(GL_TRIANGLES, ColorCube.NUM_FACES*3, GL_UNSIGNED_INT, 0); // Draw the cube's faces
//glDrawArrays(GL_TRIANGLES, 0, ColorCube.NUM_VERTS*3); // Draw the cube's faces


	glBindBuffer(GL_VERTEX_ARRAY, vao);
	glDrawArrays(GL_TRIANGLES, 0, mesh.size()*3);
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
	case PERSPECTIVE://perspective
		setCamProjection(camera, PERSPECTIVE);
		break;

	case ORTHOGRAPHIC:
		setCamProjection(camera, ORTHOGRAPHIC);
		break;

	case PARALLEL: //parallel
		setCamProjection(camera, PARALLEL);
		break;
	}

	glutPostRedisplay();
}

void createAnimationMenus() {
	GLuint menu;

	// create a parent menu
	menu = glutCreateMenu(animationMenu);
	// add entries to parent menu
	glutAddMenuEntry("Perspective", PERSPECTIVE);
	glutAddMenuEntry("Orthographic", ORTHOGRAPHIC);
	glutAddMenuEntry("Parallel ", PARALLEL);


	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void resetTransformation() {
	ColorCube.setIdentity();
}


void myKeyboard(unsigned char key, int x, int y)
{
	double delta = 0.1;
	
	switch (key) {
		// translate X
	case 'd':case 'D': camRadius += delta; 
		fprintf(stdout, "Camera rotaion radius increased, now is %2f\n", camRadius); break;
	case 'a':case 'A': 
		if (camRadius > 1.2)
		{
			camRadius -= delta;
			fprintf(stdout, "Camera rotaion radius decreased, now is %2f\n", camRadius); break;
		}
		else {
			fprintf(stdout, "Minimum Camera rotaion radius reached, press 'D' to increase radius\n"); break;
		}
		
		// translate Y
	case 'w':case 'W': camY += delta; 
		fprintf(stdout, "Camera Y position increased, now is %2f\n", camY); break;
	case 's':case 'S': camY -= delta; 
		fprintf(stdout, "Camera Y position decreased, now is %2f\n", camY); break;
		// translate Z		
	case 'e': case 'E': speed += 0.1; 
		fprintf(stdout, "Camera rotation speed increased, now is %2f\n", speed); break;
	case 'q': case 'Q': speed -= 0.1; 
		fprintf(stdout, "Camera rotation speed decreased, now is %2f\n", speed); break;

	//	// reset transformations
	//case 'R': case 'r': resetTransformation(); fprintf(stdout, "Transformations reseted\n"); break;

		// stop animation
	case ' ': stopAnimation = !stopAnimation; break;


		// exit program
	case 033:
		glDeleteProgram(program);
		glutDestroyWindow(mainWindow);
		exit(EXIT_SUCCESS);
	}



	glutPostRedisplay();
}

//----------------------------------------------------------------------------

void myReshape(int w, int h) {
	glViewport(0, 0, w, h);
}

//----------------------------------------------------------------------------

void myIdle() {
	/* change something over time*/
	if (!stopAnimation) {
		t += dt;
		//angle = t * speed;
	}

	glutPostWindowRedisplay(mainWindow);
}

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


void initModel(){

	#ifdef __linux__ 
		char* filePath = "./resources/dragon-50000.smf";
		int ifSuccess = loadSFM(filePath, mesh);
	#elif _WIN32
		
		char* filePath = "..\\src\\resources\\dragon-50000.smf";
		int ifSuccess = loadSFM(filePath, mesh);
	#endif

	if (!ifSuccess) {
		fprintf(stdout, "fail to load SMF file\n");
		exit(0);
	}

	int meshLength = mesh.size();
	int numVertPerTriangle = 3;

	int vertArrayLength = numVertPerTriangle * meshLength;

	// dynamic array
	point3* points = new point3[vertArrayLength];
	vec3* normals = new vec3[vertArrayLength];
	point3* centerOfMass = new point3[vertArrayLength];

	int sizePoints = vertArrayLength*sizeof(points[0]);
	int sizeNormals = vertArrayLength*sizeof(normals[0]);
	int sizeCenterOfMass = vertArrayLength* sizeof(centerOfMass[0]);


	for (int i = 0; i < meshLength; i++) {

		points[3 * i]     = mesh[i].vertices[0];
		points[3 * i + 1] = mesh[i].vertices[1];
		points[3 * i + 2] = mesh[i].vertices[2];

		normals[3 * i]     = mesh[i].normals[0];
		normals[3 * i + 1] = mesh[i].normals[1];
		normals[3 * i + 2] = mesh[i].normals[2];

		centerOfMass[3 * i]     = mesh[i].centerOfMass;
		centerOfMass[3 * i + 1] = mesh[i].centerOfMass;
		centerOfMass[3 * i + 2] = mesh[i].centerOfMass;
	}

	// Create a vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// total buffer size
	glBufferData(GL_ARRAY_BUFFER, sizePoints + sizeNormals + sizeCenterOfMass, NULL, GL_STATIC_DRAW);

	// points
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizePoints, points);

	// normals
	glBufferSubData(GL_ARRAY_BUFFER, sizePoints, sizeNormals, normals);

	//center of mass
	glBufferSubData(GL_ARRAY_BUFFER, sizePoints+sizeNormals, sizeCenterOfMass, centerOfMass);


	glUseProgram(program);

	// set up vertex buffer pointers
	GLuint aPosition = glGetAttribLocation(program, "aPosition");
	glEnableVertexAttribArray(aPosition);
	glVertexAttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint aNormal = glGetAttribLocation(program, "aNormal");
	glEnableVertexAttribArray(aNormal);
	glVertexAttribPointer(aNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizePoints));

	GLuint aCenterOfMass = glGetAttribLocation(program, "aCenterOfMass");
	glEnableVertexAttribArray(aCenterOfMass);
	glVertexAttribPointer(aCenterOfMass, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizePoints + sizeNormals));

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// delete dynamic array pointers
	delete[] points;
	delete[] normals;
	delete[] centerOfMass;


}

void initCamera(Camera &camera, int option)
{
	// init camera view matrix
	vec3 cameraPosition = vec3(0.0, 0.0, 10.0);
	vec3 cameraTarget = vec3(0.0, 0.0, 0.0);
	vec3 cameraUp = vec3(0.0, 1.0, 0.0);
	camera.LookAt(cameraPosition, cameraTarget, cameraUp);

	setCamProjection(camera, option);
}


void setCamProjection(Camera &camera, int option) {
	// init camera projection matrix	
	switch (option){
		case PERSPECTIVE:
			// perspective projection
		{double fovy{ 50.0 }, aspect{ 1.0 }, zNear{ 1.0 }, zFar{ 10.0 };
		camera.Perspective(fovy, aspect, zNear, zFar);
		fprintf(stdout, " Set camera to perspective mode \n");
		break;
		}

		case ORTHOGRAPHIC:
			// orthographic projection
		{double left{ -2.0 }, right{ 2.0 }, bottom{ -2.0 }, top{ 2.0 }, zNear{ 0.001 }, zFar{ 100.0 };
		camera.Ortho(left, right, bottom, top, zNear, zFar);
		fprintf(stdout, " Set camera to orthographic mode \n");
		break; }


		// parallel projection
		case PARALLEL:			
		{
			double left{ -2.0 }, right{ 2.0 }, bottom{ -2.0 }, top{ 2.0 }, zNear{ 0.001 }, zFar{ 100.0 };
			camera.Ortho(left, right, bottom, top, zNear, zFar);

			mat4 shearMatrix = Angel::identity();
			double theta{ 85 };
			double phi{ 95 };
			shearMatrix[0][2] = -1 / tan(DegreesToRadians * theta);
			//shearMatrix[0][2] = 0;
			shearMatrix[1][2] = -1 / tan(DegreesToRadians * phi);
			//shearMatrix[1][2] = 0;

			camera.projMatrix = camera.projMatrix*shearMatrix;
			fprintf(stdout, " Set camera to parallel mode \n");
			break; 
		}

	}
	
		

}

void printInstructions()
{
	fprintf(stdout, "***************************** Instructions *********************************** \n");
	fprintf(stdout, "1. Press 'W', 'S' for increasing and decreasing the camera's Y position \n");
	fprintf(stdout, "2. Press 'A', 'D' for increasing and decreasing the camera's rotation raidus \n");
	fprintf(stdout, "3. Press 'Q', 'E' to increasing and decreasing the camera's rotation speed \n");
	fprintf(stdout, "4. Press 'SPACE' to pause/resume animation \n");
	fprintf(stdout, "5. Use mouse right click to change camera's projection mode \n");
	fprintf(stdout, "5. Press 'ESC' to exit program \n");

}


void initScene() {
	printInstructions();



	initModel();
	initCamera(camera, PERSPECTIVE);
	// init color cube
	//initColorCube();
	//ColorCube.init(program);
}



//------------------------ Main function ------------------------------------------
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);	
	glFrontFace(GL_CCW);

	///********* main window  ****************/ 
	mainWindow = createOpenGLContext("ICG HW6 - Lighting, Materials, Gouraud and Phong Shading", 200, 0, 500, 500);

	glEnable(GL_DEPTH_TEST); //! Depth test must be enabled after the glewInit(), or it doesn't work
	assignGlutFunctions(displayMainWindow, myKeyboard, nullptr, nullptr, nullptr, myIdle, createAnimationMenus);
	initScene();

	glutMainLoop();
	int k;
	cin >> k;
	return 0;

}
