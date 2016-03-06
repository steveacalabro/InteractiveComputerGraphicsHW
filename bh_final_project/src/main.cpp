// HW6
#include "main.h"
#include "BoxGeometry.h"

using namespace std;


//-------------------- Gloabal variables ----------------------------------------------
GLenum err;

// global shader program
Program program;


// global vertex array object
GLuint vao, axisVao, controlPointsVao;

// global frame buffer object
GLuint fbo;

// global render buffer object
GLuint colorRBO, depthRBO;


// Bezir curve control points
vector<point3> controlPoints;
vector<point3> controlColors;
vector<float> pointSize;

int selectedPoint{ 0 };
//double deltaX, deltaY, deltaZ;
double offset = 0.01;
int resolution{ 10 };
// main window id
int mainWindow;

int winWidth{ 800 }, winHeight{ 600 };

enum class VBO_OPTION { VERTEX, COLOR };

// for right click menu
bool onPerspective{ true };

// mode, camera and scene class
//BoxGeometry ColorCube;
Camera camera;
Scene scene;
Mesh mesh;

MeshObject base;
MeshObject arm1;
MeshObject arm2;
MeshObject arm3;

enum {BACKGOURND, ARM1, ARM2, ARM3};
int selectedObj = BACKGOURND;
double armRotSpeed{ 10.0 };

double camRadius{ 1.1 }, camY{ 3.0 };
double speed{ 0.1 };

double lightRadius{ 3.0 }, lightX{ 0.0 }, lightY{ 0.0 }, lightZ{ 0.0 }, lightAngle{ 0.0 };



double t{ 0.0 }, dt{ 0.01 };
//double angle{ 0.0 };
bool stopAnimation = false;

enum { PERSPECTIVE, ORTHOGRAPHIC, PARALLEL };
enum { METAL, PLASTIC, GOLD };
enum { GOURAUD, PHONG, FLAT };
enum { CYLINDER = 2, PLANE = 3 };

enum { WOOD = 4, CLOTH = 5 };
// textures id
GLuint woodTex, clothTex;

int materialOption{ METAL };
int shaderOption{ PHONG };
int texMappingOption{ CYLINDER };


float *currentState = &arm3.translationMatrix[1][3];  // y 
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

	int numVertComponents; // assumes all elements are floats
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
	program.lineShader = InitShader("./shader/vLineShader.glsl", "./shader/fLineShader.glsl");
	program.polygonShader = InitShader("./shader/vPolygonShader.glsl", "./shader/fPolygonShader_linux.glsl");
	program.pickingShader = InitShader("./shader/vPickingShader.glsl", "./shader/fPickingShader.glsl");
#elif _WIN32
	program.lineShader = InitShader("..\\src\\shader\\vLineShader.glsl", "..\\src\\shader\\fLineShader.glsl");
	program.polygonShader = InitShader("..\\src\\shader\\vPolygonShader.glsl", "..\\src\\shader\\fPolygonShader.glsl");
	program.pickingShader = InitShader("..\\src\\shader\\vPickingShader.glsl", "..\\src\\shader\\fPickingShader.glsl");

#endif

	glUseProgram(program.polygonShader);
	glClearColor(0.0, 0.0, 0.0, 1.0); // black background
}


// recursive compostive model transformation, from the lowest level to the highest level
void transformToWorld(MeshObject &meshObject, mat4 &finalMatrix)
{

	// if it has a parent
	if(meshObject.parent)
	{
		transformToWorld(*meshObject.parent, finalMatrix);
	}

	//! the multiplication is from left to right, i.e., parent's parent * parent* self, 
	//! so the muliplication should start from the upper most parent to the lower most
	finalMatrix *= meshObject.compositeMatrix;
}


//----------------------------------------------------------------------------
void renderMeshObject(MeshObject &meshObject, GLuint shaderProgram = program.polygonShader) {
	glUseProgram(shaderProgram);
	
	meshObject.updateMatrix();
	Mesh &mesh = meshObject.mesh;

	GLuint &vao = meshObject.vao.id;
	GLuint &pickingVao = meshObject.pickingVao.id;
	vec3 orgSurfaceColor = meshObject.objectColor[RENDER];

	// recursive compostive model transformation
	mat4 finalModelMatrix;
	transformToWorld(meshObject, finalModelMatrix);
	
	// contruct modelview and modelviewProjection matrix
	mat4 MV = camera.viewMatrix * scene.compositeMatrix * finalModelMatrix;
	mat4 MVP = camera.projMatrix * MV;
	

	// send model view projection matrix to shader
	GLuint u_MVP = glGetUniformLocation(shaderProgram, "u_MVP");
	glUniformMatrix4fv(u_MVP, 1, GL_TRUE, MVP); // mat.h is row major, so use GL_TRUE to transpsoe t

	// send model view matrix to shader
	/*mat4 MV = camera.viewMatrix * scene.compositeMatrix * scaleMatrix;*/
	GLuint u_MV = glGetUniformLocation(shaderProgram, "u_MV");
	glUniformMatrix4fv(u_MV, 1, GL_TRUE, MV); // mat.h is row major, so use GL_TRUE to transpsoe t

	if (shaderProgram == program.polygonShader) {
		// send light position to shader
		vec4 lightPos = vec4(lightX, lightY, lightZ, 1.0);
		GLuint u_lightPos = glGetUniformLocation(shaderProgram, "u_lightPos");
		glUniform4fv(u_lightPos, 1, lightPos); // mat.h is row major, so use GL_TRUE to transpsoe t


		// send material option to shader
		GLuint u_material = glGetUniformLocation(shaderProgram, "u_material");
		glUniform1i(u_material, materialOption);

		// send object original surface color to shader
		GLuint u_Color = glGetUniformLocation(shaderProgram, "u_Color");
		glUniform3fv(u_Color, 1, orgSurfaceColor);

		// send shader option to shader
		GLuint u_shadingModel = glGetUniformLocation(shaderProgram, "u_shadingModel");
		glUniform1i(u_shadingModel, shaderOption);


		// send texture mapping option to shader
		GLuint u_texMappingModel = glGetUniformLocation(shaderProgram, "u_texMappingModel");
		glUniform1i(u_texMappingModel, texMappingOption);


		glBindVertexArray(vao);
	}
	else if (shaderProgram == program.pickingShader) {
		glBindVertexArray(pickingVao);

		// send object original surface color to shader
		GLuint u_Color = glGetUniformLocation(shaderProgram, "u_Color");
		glUniform3fv(u_Color, 1, meshObject.objectColor[ORIGINAL]);
	}


	glDrawArrays(GL_TRIANGLES, 0, mesh.size() * 3);

}

void renderAxis() {
	glUseProgram(program.lineShader);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 scaleMatrix = Scale(2.0, 2.0, 2.0);

	// axis is defined in World space not need to apply scene.compositeMatrix
	mat4 MVP = camera.projViewMatrix  * scaleMatrix;

	// send model view projection matrix to shader
	GLuint u_MVP = glGetUniformLocation(program.lineShader, "u_MVP");
	glUniformMatrix4fv(u_MVP, 1, GL_TRUE, MVP); // mat.h is row major, so use GL_TRUE to transpsoe t

	glBindVertexArray(axisVao);

	glEnable(GL_PROGRAM_POINT_SIZE);
	glDrawArrays(GL_LINES, 0, 6);
}

void rendeControlPoints() {
	initControlPoints();

	mat4 scaleMatrix = Scale(0.5, 0.5, 0.5);
	mat4 translationMatrix = Translate(-2.0, -2.0, 0.0);
	glUseProgram(program.lineShader);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mat4 MVP = camera.projViewMatrix * scene.compositeMatrix * scaleMatrix * translationMatrix;

	// send model view projection matrix to shader
	GLuint u_MVP = glGetUniformLocation(program.lineShader, "u_MVP");
	glUniformMatrix4fv(u_MVP, 1, GL_TRUE, MVP); // mat.h is row major, so use GL_TRUE to transpsoe t

	glBindVertexArray(controlPointsVao);

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(0x8861);
	glDrawArrays(GL_POINTS, 0, controlPoints.size());
}


void updateScene() {
	// update camera rotation and height
	double angle = t * speed;
	double rotationCenterX = 0.0;
	double rotationCenterZ = 0.0;

	double camX = rotationCenterX + camRadius*cos(DegreesToRadians * 90 + angle);
	double camZ = rotationCenterZ + camRadius*sin(DegreesToRadians * 90 + angle);

	vec4 camPos = vec4(camX, camY, camZ, 1.0);
	vec4 camTarget = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 camUp = vec4(0.0, 1.0, 0.0, 0.0);

	camera.position = camPos;
	camera.target = camTarget;
	camera.up = camUp;

	//camera.LookAt(camPos, camTarget, camUp);

	// update MVP
	camera.updateMatrix();
	scene.updateMatrix();
	base.updateMatrix();
	arm1.updateMatrix();
	arm2.updateMatrix();
	arm3.updateMatrix();

	// update light rotation and height
	lightX = rotationCenterX + lightRadius*cos(DegreesToRadians * lightAngle);
	lightZ = rotationCenterZ + lightRadius*sin(DegreesToRadians * lightAngle);
}

void renderToBackBuffer() {
	//Bind 0, which means render to back buffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderMeshObject(base);
	renderMeshObject(arm1, program.polygonShader);
	renderMeshObject(arm2, program.polygonShader);
	renderMeshObject(arm3, program.polygonShader);
	renderAxis();
}

void renderToPickingBuffer() {
	//and now you can render to the FBO (also called RenderBuffer)
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	//glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, winWidth, winHeight);

	//renderMeshObject(base, program.pickingShader);
	renderMeshObject(arm1, program.pickingShader);
	renderMeshObject(arm2, program.pickingShader);
	renderMeshObject(arm3, program.pickingShader);

}

void render() {

	updateScene();
	renderToPickingBuffer();
	renderToBackBuffer();

}



void displayMainWindow(void)
{

	render();

	glutSwapBuffers();

}



void projectionMenu(int option)
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


void materialMenue(int option)
{
	switch (option)
	{
	case METAL://perspective
		materialOption = METAL;
		fprintf(stdout, " Set material to metal\n");
		break;

	case PLASTIC:
		materialOption = PLASTIC;
		fprintf(stdout, " Set material to plastic\n");
		break;

	case GOLD: //parallel
		materialOption = GOLD;
		fprintf(stdout, " Set material to gold\n");
		break;
	}

	glutPostRedisplay();
}

void shadingMenu(int option)
{
	switch (option)
	{
	case GOURAUD:
		shaderOption = GOURAUD;
		fprintf(stdout, " Set shading model to Gouraud\n");
		break;

	case PHONG:
		shaderOption = PHONG;
		fprintf(stdout, " Set shading model to Phong\n");
		break;

	case FLAT:
		shaderOption = FLAT;
		fprintf(stdout, " Set shading model to Flat \n");
		break;
	}
	glutPostRedisplay();
}

void texMappingMenu(int option)
{
	switch (option)
	{
	case CYLINDER:
		texMappingOption = CYLINDER;
		fprintf(stdout, " Set texture mapping to cylindrical\n");
		break;

	case PLANE:
		texMappingOption = PLANE;
		fprintf(stdout, " Set texture mapping to planar\n");
		break;
	}
	glutPostRedisplay();
}

void texImgMenu(int option)
{
	switch (option)
	{
	case WOOD:
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTex);
		//glUniform1i(glGetUniformLocation(program.polygonShader, "texture"), 0);
		fprintf(stdout, " Set texture to wood\n");
		break;

	case CLOTH:
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, clothTex);
		//glUniform1i(glGetUniformLocation(program.polygonShader, "texture"), 0);
		fprintf(stdout, " Set texture to cloth\n");
		break;
	}
	glutPostRedisplay();
}


void parentMenu(int option)
{
	glutPostRedisplay();
}

void createAnimationMenus() {
	GLuint projectionSubMenu;
	GLuint materialSubMenue;
	GLuint shadingSubMenue;
	GLuint texMappingSubMenu;


	// projection mode
	projectionSubMenu = glutCreateMenu(projectionMenu);
	glutAddMenuEntry("Perspective", PERSPECTIVE);
	glutAddMenuEntry("Orthographic", ORTHOGRAPHIC);
	glutAddMenuEntry("Parallel ", PARALLEL);


	// material
	materialSubMenue = glutCreateMenu(materialMenue);
	glutAddMenuEntry("Metal", METAL);
	glutAddMenuEntry("Plastic", PLASTIC);
	glutAddMenuEntry("Gold ", GOLD);


	// shading model
	shadingSubMenue = glutCreateMenu(shadingMenu);
	glutAddMenuEntry("Gouraud", GOURAUD);
	glutAddMenuEntry("Phong", PHONG);
	glutAddMenuEntry("Flat", FLAT);

	// texture mapping model
	texMappingSubMenu = glutCreateMenu(texMappingMenu);
	glutAddMenuEntry("Cylindrical", CYLINDER);
	glutAddMenuEntry("Planar", PLANE);

	// texture image selection
	int textureImgSubMenu = glutCreateMenu(texImgMenu);
	glutAddMenuEntry("Wood", WOOD);
	glutAddMenuEntry("Cloth", CLOTH);


	// create a parent menu and add entries to parent menu
	glutCreateMenu(parentMenu);
	glutAddSubMenu("Projection Mode", projectionSubMenu);
	glutAddSubMenu("Shading Model", shadingSubMenue);
	glutAddSubMenu("Material", materialSubMenue);
	glutAddSubMenu("Texture image", textureImgSubMenu);
	glutAddSubMenu("Texture mapping Model", texMappingSubMenu);


	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

//void resetTransformation() {
//	ColorCube.setIdentity();
//}
void myMouse(GLint button, GLint state, GLint x, GLint y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		vector<GLubyte> pixels(4);

		// Set up to read from the renderbuffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
		glReadBuffer(GL_COLOR_ATTACHMENT0);

		// here the x, y are in image space where the origin is at the lower left corner
		//! GL_RGBA8 returns zeros somehow
		glReadPixels(x, winHeight - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);

		// if hit base
		if (pixels[0] == 255) {
			arm1.objectColor[RENDER] = arm1.objectColor[PICKED];
			arm2.objectColor[RENDER] = color3(1.0, 1.0, 1.0);
			arm3.objectColor[RENDER] = color3(1.0, 1.0, 1.0);
			fprintf(stdout, "arm 1 seleted \n");

			selectedObj = ARM1;
			currentState = nullptr;
		}
		// if hit arm1
		else if (pixels[1] == 255) {
			arm2.objectColor[RENDER] = arm2.objectColor[PICKED];
			arm1.objectColor[RENDER] = color3(1.0, 1.0, 1.0);
			arm3.objectColor[RENDER] = color3(1.0, 1.0, 1.0);
			fprintf(stdout, "arm 2 seleted \n");
			selectedObj = ARM2;
			currentState = nullptr;
		}
		// if hit arm2
		else if (pixels[2] == 255) {
			arm3.objectColor[RENDER] = arm3.objectColor[PICKED];
			arm2.objectColor[RENDER] = color3(1.0, 1.0, 1.0);
			arm1.objectColor[RENDER] = color3(1.0, 1.0, 1.0);
			fprintf(stdout, "arm 3 seleted \n");

			selectedObj = ARM3;
			currentState = &arm3.translationMatrix[1][3];
		}
		// if hit the background
		else
		{
			arm3.objectColor[RENDER] = color3(1.0, 1.0, 1.0);
			arm1.objectColor[RENDER] = color3(1.0, 1.0, 1.0);
			arm2.objectColor[RENDER] = color3(1.0, 1.0, 1.0);
			fprintf(stdout, "No object selected \n");
			currentState = nullptr;
			selectedObj = BACKGOURND;
		}
	}

}


void myKeyboard(unsigned char key, int x, int y)
{
	double delta = 0.1; double theta = 10.0;
	static int rotateCount = 0;
	switch (key) {
		// camera radius
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

		// camera height
	case 'w':case 'W': camY += delta;
		fprintf(stdout, "Camera Y position increased, now is %2f\n", camY); break;
	case 's':case 'S': camY -= delta;
		fprintf(stdout, "Camera Y position decreased, now is %2f\n", camY); break;


		// rotation speed	
	case 'e': case 'E': speed += 0.1;
		fprintf(stdout, "Camera rotation speed increased, now is %2f\n", speed); break;
	case 'q': case 'Q': speed -= 0.1;
		fprintf(stdout, "Camera rotation speed decreased, now is %2f\n", speed); break;


		// light radius
	case 'z':case 'Z': lightRadius += delta;
		fprintf(stdout, "Camera rotaion radius increased, now is %2f\n", lightRadius); break;
	case 'x':case 'X':
		if (lightRadius > 1.2)
		{
			lightRadius -= delta;
			fprintf(stdout, "Light rotaion radius decreased, now is %2f\n", lightRadius); break;
		}
		else {
			fprintf(stdout, "Minimum light rotaion radius reached, press 'D' to increase radius\n"); break;
		}


		// light height
	case 'c':case 'C': lightY += delta;
		fprintf(stdout, "Light Y position increased, now is %2f\n", lightY); break;
	case 'v':case 'V': lightY -= delta;
		fprintf(stdout, "Light Y position decreased, now is %2f\n", lightY); break;

		// light angle
	case 'b':case 'B': lightAngle += theta;
		fprintf(stdout, "Light angle increased, now is %2f\n", lightAngle); break;
	case 'n':case 'N': lightAngle -= theta;
		fprintf(stdout, "Light angle decreased, now is %2f\n", lightAngle); break;


		// robot arm angle 
	case 't':case 'T':
		if (currentState)
		{
			// selected arm3
			if (*currentState < 39)
			{
				*currentState += 2.0;
				fprintf(stdout, "Arm3 Y position increased, now is %2f\n", *currentState);
				break;
			}
			else
			{
				fprintf(stdout, "Arm3 translation reach limit \n");
			}
		}
		if (selectedObj == ARM1)
		{
			arm1.rotate(0, armRotSpeed, 0);
			fprintf(stdout, "rotate arm1 \n");
			break;
		}
		if (selectedObj == ARM2)
		{

			if(armRotSpeed*rotateCount < 260)
			{
				arm2.rotate(0, armRotSpeed, 0);
				fprintf(stdout, "rotate arm2 \n"); 
				rotateCount++;
			}else
			{
				fprintf(stdout, "Arm2 rotation reach limit \n");
			}
			
			break;
		}

	case 'y':case 'Y':
		
		if (currentState)
		{
			// selected arm3
			if(*currentState > -39)
			{
				*currentState -= 2.0;
				fprintf(stdout, "Arm3 Y position increased, now is %2f\n", *currentState);
				break;
			}
			else
			{
				fprintf(stdout, "Arm3 translation reach limit \n");
			}
		}
		if (selectedObj == ARM1)
		{
			arm1.rotate(0, -armRotSpeed, 0);
			fprintf(stdout, "rotate arm1 \n");
			break;
		}
		if (selectedObj == ARM2)
		{
		
			if (armRotSpeed*rotateCount > -80)
			{
				arm2.rotate(0, -armRotSpeed, 0);
				fprintf(stdout, "rotate arm2 \n"); 
				rotateCount--;
			}
			else
			{
				fprintf(stdout, "Arm2 rotation reach limit \n");
			}
		}
		break;


		//	// reset camera view and projection
	case 'R': case 'r':
		camera.reset();
		camRadius = 6.0;
		camY = 0.0;
		fprintf(stdout, "Camera reseted\n"); break;

		// stop animation
	case ' ': stopAnimation = !stopAnimation; break;


		// exit program
	case 033:
		glDeleteProgram(program.polygonShader);
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
		err = glewInit();
	}



	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %p\n", glewGetErrorString(err));
	}

	initShaderProgram(); // note that each windows should have a shader program
	return windowID;
}

void initAxis()
{
	glUseProgram(program.lineShader);

	point3 points[6] = { point3(0.0, 0.0, 0.0), point3(1.0, 0.0, 0.0), // x axis
		point3(0.0, 0.0, 0.0),  point3(0.0, 1.0, 0.0), // y axis
		point3(0.0, 0.0, 0.0),  point3(0.0, 0.0, 1.0) // z axis
	};

	color3 colors[6] = { color3(1.0, 0.0, 0.0), color3(1.0, 0.0, 0.0), // x axis
		color3(0.0, 1.0, 0.0), color3(0.0, 1.0, 0.0),  // y axis
		color3(0.0, 0.0, 1.0), color3(0.0, 0.0, 1.0)  // z axis
	};

	// Create a vertex array object
	glGenVertexArrays(1, &axisVao);
	glBindVertexArray(axisVao);

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



	// set up vertex arrays
	GLuint aPosition = glGetAttribLocation(program.lineShader, "aPosition");
	glEnableVertexAttribArray(aPosition);
	glVertexAttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));
	GLuint aColor = glGetAttribLocation(program.lineShader, "aColor");
	glEnableVertexAttribArray(aColor);
	glVertexAttribPointer(aColor, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void initControlPoints()
{
	glUseProgram(program.lineShader);

	vector<point3> &colors = controlColors;
	vector<point3> &points = controlPoints;


	// initialize colors if it is not initialized
	if (colors.size() == 0) {
		for (int i = 0; i < points.size(); i++) {
			colors.push_back(point3(1.0, 1.0, 1.0));
		}
	}

	if (pointSize.size() == 0) {
		for (int i = 0; i < points.size(); i++) {
			pointSize.push_back(5.0);
		}
	}

	int sizePoints = points.size()*sizeof(points[0]);
	int sizeColors = colors.size()*sizeof(colors[0]);
	int sizePointSize = pointSize.size()*sizeof(pointSize[0]);

	// Create a vertex array object
	glGenVertexArrays(1, &controlPointsVao);
	glBindVertexArray(controlPointsVao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizePoints + sizeColors + sizePointSize, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizePoints, &points[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizePoints, sizeColors, &colors[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizePoints + sizeColors, sizePointSize, &pointSize[0]);


	// set up vertex arrays
	GLuint aPosition = glGetAttribLocation(program.lineShader, "aPosition");
	glEnableVertexAttribArray(aPosition);
	glVertexAttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint aColor = glGetAttribLocation(program.lineShader, "aColor");
	glEnableVertexAttribArray(aColor);
	glVertexAttribPointer(aColor, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizePoints));

	GLuint aPointSize = glGetAttribLocation(program.lineShader, "aPointSize");
	glEnableVertexAttribArray(aPointSize);
	glVertexAttribPointer(aPointSize, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizePoints + sizeColors));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void initMesh(MeshObject &meshObject) {

	Mesh &mesh = meshObject.mesh;
	int meshLength = mesh.size();
	int numVertPerTriangle = 3;

	int vertArrayLength = numVertPerTriangle * meshLength;

	// dynamic array
	point3* points = new point3[vertArrayLength];
	vec3* normals = new vec3[vertArrayLength];
	vec3* triangleNormal = new vec3[vertArrayLength];
	point3* centerOfMass = new point3[vertArrayLength];
	vec3* texCoord = new vec3[vertArrayLength];

	int sizePoints = vertArrayLength*sizeof(points[0]);
	int sizeNormals = vertArrayLength*sizeof(normals[0]);
	int sizeTriangleNormal = vertArrayLength*sizeof(triangleNormal[0]);
	int sizeCenterOfMass = vertArrayLength* sizeof(centerOfMass[0]);
	int sizeTexCoord = vertArrayLength* sizeof(texCoord[0]);

	for (int i = 0; i < meshLength; i++) {

		points[3 * i] = mesh[i].vertices[0];
		points[3 * i + 1] = mesh[i].vertices[1];
		points[3 * i + 2] = mesh[i].vertices[2];

		normals[3 * i] = mesh[i].normals[0];
		normals[3 * i + 1] = mesh[i].normals[1];
		normals[3 * i + 2] = mesh[i].normals[2];

		triangleNormal[3 * i] = mesh[i].triangleNormal;
		triangleNormal[3 * i + 1] = mesh[i].triangleNormal;
		triangleNormal[3 * i + 2] = mesh[i].triangleNormal;

		centerOfMass[3 * i] = mesh[i].centerOfMass;
		centerOfMass[3 * i + 1] = mesh[i].centerOfMass;
		centerOfMass[3 * i + 2] = mesh[i].centerOfMass;

		texCoord[3 * i] = mesh[i].texCoord[0];
		texCoord[3 * i + 1] = mesh[i].texCoord[1];
		texCoord[3 * i + 2] = mesh[i].texCoord[2];



	}

	// Create a vertex array object
	glGenVertexArrays(1, &meshObject.vao.id);
	glBindVertexArray(meshObject.vao.id);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// total buffer size
	glBufferData(GL_ARRAY_BUFFER, sizePoints + sizeNormals + sizeCenterOfMass + sizeTriangleNormal + sizeTexCoord, NULL, GL_STATIC_DRAW);

	// points
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizePoints, points);

	// normals
	glBufferSubData(GL_ARRAY_BUFFER, sizePoints, sizeNormals, normals);

	//center of mass
	glBufferSubData(GL_ARRAY_BUFFER, sizePoints + sizeNormals, sizeCenterOfMass, centerOfMass);

	// facet normal for flat shading 
	glBufferSubData(GL_ARRAY_BUFFER, sizePoints + sizeNormals + sizeCenterOfMass, sizeTriangleNormal, triangleNormal);

	// facet texture coord
	glBufferSubData(GL_ARRAY_BUFFER, sizePoints + sizeNormals + sizeCenterOfMass + sizeTriangleNormal, sizeTexCoord, texCoord);

	// set up vertex buffer pointers
	GLuint aPosition = glGetAttribLocation(program.polygonShader, "aPosition");
	glEnableVertexAttribArray(aPosition);
	glVertexAttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint aNormal = glGetAttribLocation(program.polygonShader, "aNormal");
	glEnableVertexAttribArray(aNormal);
	glVertexAttribPointer(aNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizePoints));

	GLuint aCenterOfMass = glGetAttribLocation(program.polygonShader, "aCenterOfMass");
	glEnableVertexAttribArray(aCenterOfMass);
	glVertexAttribPointer(aCenterOfMass, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizePoints + sizeNormals));

	GLuint aFlatNormal = glGetAttribLocation(program.polygonShader, "aFlatNormal");
	glEnableVertexAttribArray(aFlatNormal);
	glVertexAttribPointer(aFlatNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizePoints + sizeNormals + sizeCenterOfMass));

	GLuint aTexCoord = glGetAttribLocation(program.polygonShader, "aTexCoord");
	glEnableVertexAttribArray(aTexCoord);
	glVertexAttribPointer(aTexCoord, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizePoints + sizeNormals + sizeCenterOfMass + sizeTriangleNormal));
	glBindBuffer(GL_ARRAY_BUFFER, 0);



	/************************* For picking render buffer ***********************************/
	// create vao for picking buffer
	glGenVertexArrays(1, &meshObject.pickingVao.id);
	glBindVertexArray(meshObject.pickingVao.id);

	// Create and initialize a buffer object
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	// total buffer size
	glBufferData(GL_ARRAY_BUFFER, sizePoints, points, GL_STATIC_DRAW);

	// set up vertex buffer pointers
	aPosition = glGetAttribLocation(program.pickingShader, "aPosition");
	glEnableVertexAttribArray(aPosition);
	glVertexAttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));


	// delete dynamic array pointers
	delete[] points;
	delete[] normals;
	delete[] centerOfMass;
	delete[] triangleNormal;
	delete[] texCoord;
}

void initModel() {
	glUseProgram(program.polygonShader);

#ifdef __linux__ 
	char* basePath = "./resources/base.obj";
	char* arm1Path = "./resources/arm1.obj";
	char* arm2Path = "./resources/arm2.obj";
	char* arm3Path = "./resources/arm3.obj";
#elif _WIN32

	char* basePath = "..\\src\\resources\\base.obj";
	char* arm1Path = "..\\src\\resources\\arm1.obj";
	char* arm2Path = "..\\src\\resources\\arm2.obj";
	char* arm3Path = "..\\src\\resources\\arm3.obj";


#endif

	loadSFM(basePath, base.mesh);
	initMesh(base);
	
	base.objectColor[ORIGINAL] = color3(1.0, 1.0, 1.0);
	base.objectColor[PICKED] = color3(1.0, 1.0, 1.0);
	base.objectColor[RENDER] = base.objectColor[ORIGINAL];


	loadSFM(arm1Path, arm1.mesh);
	initMesh(arm1);
	arm1.modelOrigin = vec3(-245, -9, 455);

	//! careful, this translation matrix is from child model space to parent model space

	arm1.translate(arm1.modelOrigin); // since arm1 does not have a parent, its parent is World
	arm1.objectColor[ORIGINAL] = color3(1.0, 0.0, 0.0);
	arm1.objectColor[PICKED] = color3(1.0, 0.5, 0.5);
	arm1.objectColor[RENDER] = color3(1.0, 1.0, 1.0);

	loadSFM(arm2Path, arm2.mesh);
	initMesh(arm2);
	//! origin in world space
	arm2.modelOrigin = vec3(-243, 0, 0);
	arm2.parent = &arm1;
	//! careful, this translation matrix is from child model space to parent model space
	arm2.translate(arm2.modelOrigin - (*arm2.parent).modelOrigin); // arm2's parent is arm1
	arm2.objectColor[ORIGINAL] = color3(0.0, 1.0, 0.0);
	arm2.objectColor[PICKED] = color3(1.0, 0.5, 0.5);
	arm2.objectColor[RENDER] = color3(1.0, 1.0, 1.0);


	loadSFM(arm3Path, arm3.mesh);
	initMesh(arm3);
	arm3.modelOrigin = vec3(219, 60, 8.92);
	arm3.parent = &arm2;

	//! careful, this translation matrix is from child model space to parent model space
	arm3.translate(arm3.modelOrigin - (*arm3.parent).modelOrigin); // arm3's parent is arm3
	arm3.objectColor[ORIGINAL] = color3(0.0, 0.0, 1.0);
	arm3.objectColor[PICKED] = color3(1.0, 0.5, 0.5);
	arm3.objectColor[RENDER] = color3(1.0, 1.0, 1.0);

	// set the scale matrix of the scene, since the models units are in mm, and world scale is -1 ~ 1
	scene.scale(0.005, 0.005, 0.005);

	scene.translate(219, 9, -486);

}



void initCamera(Camera &camera, int option)
{

	// init camera view matrix is done in updateScene
	/*vec3 cameraPosition = vec3(0.0, 0.0, 10.0);
	vec3 cameraTarget = vec3(0.0, 0.0, 0.0);
	vec3 cameraUp = vec3(0.0, 1.0, 0.0);
	camera.LookAt(cameraPosition, cameraTarget, cameraUp);*/

	setCamProjection(camera, option);
}


void setCamProjection(Camera &camera, int option) {
	// init camera projection matrix	
	switch (option) {
		// perspective projection
	case PERSPECTIVE:
	{
		double fovy{ 80.0 }, aspect{ 1.0 }, zNear{ 1.0 }, zFar{ 50.0 };
		camera.Perspective(fovy, aspect, zNear, zFar);
		fprintf(stdout, " Set camera to perspective mode \n");
		break;
	}

	// orthographic projection
	case ORTHOGRAPHIC:
	{
		double left{ -2.0 }, right{ 2.0 }, bottom{ -2.0 }, top{ 2.0 }, zNear{ 0.001 }, zFar{ 100.0 };
		camera.Ortho(left, right, bottom, top, zNear, zFar);
		fprintf(stdout, " Set camera to orthographic mode \n");
		break;
	}


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
void resetControlPoint(int selectedPoint) {
	// change the color of the selected point to red
	controlColors[selectedPoint] = color3(1.0, 1.0, 1.0);
	pointSize[selectedPoint] = 5.0;
}

void selectControlPoint(int selectedPoint) {
	// change the color of the selected point to red
	controlColors[selectedPoint] = color3(1.0, 0.0, 0.0);
	pointSize[selectedPoint] = 8.0;


}

void translateControlPoint(int selectedPoint, double x, double y, double z) {

	controlPoints[selectedPoint] += vec3(x, y, z);

}


void printInstructions()
{
	fprintf(stdout, "***************************** Instructions *********************************** \n");


	int item = 1;
	fprintf(stdout, "%d. Press 'W', 'S' for increasing and decreasing the camera's Y position \n", item); item++;
	fprintf(stdout, "%d. Press 'A', 'D' for increasing and decreasing the camera's rotation raidus \n", item); item++;
	fprintf(stdout, "%d. Press 'Q', 'E' to increasing and decreasing the camera's rotation speed \n", item); item++;

	fprintf(stdout, "%d. Press 'Z', 'X' to increasing and decreasing the light's rotation raidus \n", item); item++;
	fprintf(stdout, "%d. Press 'C', 'V' to increasing and decreasing the light's Y position \n", item); item++;
	fprintf(stdout, "%d. Press 'B', 'N' to increasing and decreasing the light's rotation anglen \n", item); item++;

	fprintf(stdout, "%d. Press 'R' to reset camera \n", item); item++;
	fprintf(stdout, "%d. Press 'SPACE' to pause/resume animationn \n", item); item++;

	fprintf(stdout, "%d. Use mouse right click to change camera's projection mode and shading model\n", item); item++;
	fprintf(stdout, "%d. Press 'ESC' to exit program \n", item);
}


void initPickingFBO() {
	//RGBA8 RenderBuffer, 24 bit depth RenderBuffer, 256x256
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//Create and attach a color buffer for picking
	glGenRenderbuffers(1, &colorRBO);

	//We must bind color_rb before we call glRenderbufferStorage
	glBindRenderbuffer(GL_RENDERBUFFER, colorRBO);

	//The storage format is RGBA8
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, winWidth, winHeight);

	//Attach color buffer to FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_RENDERBUFFER, colorRBO);

	//------------------------- depth buffer object --------------------------//
	glGenRenderbuffers(1, &depthRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, winWidth, winHeight);

	//-------------------------
	//Attach depth buffer to FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, depthRBO);

	glEnable(GL_DEPTH_TEST);





}


GLuint createTexture(char* imagePath, int textureSize = 256)
{
	/**************checker board image ******************/
	//GLubyte image[64][64][3];
	// Create a 64 x 64 checkerboard pattern
	//for (int i = 0; i < 64; i++) {
	//	for (int j = 0; j < 64; j++) {
	//		GLubyte c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
	//		image[i][j][0] = c;
	//		image[i][j][1] = c;
	//		image[i][j][2] = c;
	//	}
	//}
	//char* imagePath = "..\\src\\resources\\texture.txt";
	//Image image;
	//loadImage(imagePath, image);
	/************** BMP texture image ******************/
	unsigned char * image = loadBMP(imagePath);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[0].size(),
	//	image.size(), 0, GL_RGB, GL_FLOAT, &image[0][0][0]);


	//! refer to https://www.opengl.org/wiki/Common_Mistakes
	//! if we set GL_RGB or GLBGR the driver converts your GL_RGB or GL_BGR to what the GPU prefers, which typically is BGRA.
	//! so it's better to tell the driver that we have 3 channels, each channle has 8 bits
	//!  We should also state that on some platforms, such as Windows, GL_BGRA for the pixel upload format is preferred.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, textureSize, textureSize, 0, GL_BGR, GL_UNSIGNED_BYTE, image);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	return texture;
}


void initTextures()
{

#ifdef __linux__ 
	woodTex = createTexture("./resources/wood.bmp");
	clothTex = createTexture("./resources/cloth.bmp");
#elif _WIN32
	woodTex = createTexture("..\\src\\resources\\wood.bmp");
	clothTex = createTexture("..\\src\\resources\\cloth.bmp");
#endif
	// active the wood texture by default
	glActiveTexture(GL_TEXTURE0);


	// Set the value of the fragment shader texture sampler variable
	// ("texture") to the the appropriate texture unit. In this case,
	// zero, for GL_TEXTURE0 which was previously set by calling
	// glActiveTexture().
	glUniform1i(glGetUniformLocation(program.polygonShader, "texture"), 0);
}


void initScene() {
	printInstructions();

	initAxis();
	initModel();

	initTextures();

	initCamera(camera, PERSPECTIVE);

	initPickingFBO();

}



//------------------------ Main function ------------------------------------------
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glFrontFace(GL_CCW);

	///********* main window  ****************/ 
	mainWindow = createOpenGLContext("ICG HW9b - 3D procedural texturing", 200, 0, winWidth, winHeight);

	glEnable(GL_DEPTH_TEST); //! Depth test must be enabled after the glewInit(), or it doesn't work
	assignGlutFunctions(displayMainWindow, myKeyboard, myMouse, nullptr, nullptr, myIdle, createAnimationMenus);
	initScene();

	glutMainLoop();
	int k;
	cin >> k;
	return 0;

}
