#ifndef __MAIN_H__
#define __MAIN_H__

#include "Angel.h"
//#include "BoxGeometry.h"
#include "Camera.h"
#include "Scene.h"

#include <vector>
#include <iostream>
#include <fstream>

using namespace std;
struct VertexArrayObject
{
	GLuint id;
	GLuint vertVBO;
	//GLuint colorVBO;
	GLuint EBO;
};

struct IndexBufferObject
{
	GLuint id;
	GLuint numIndices;
};


// 3D triangle, each triangle has 3 vertices and 3 normals
struct Triangle3D
{
	int faces[3]; // face id in SMF model
	vec3 vertices[3]; // 3 vertex coord
	vec3 normals[3]; // 3 normal 
	vec3 triangleNormal; // the normal of the triangle plane
	vec3 centerOfMass;
	
};

// a mesh is a seris of 3d triangles
typedef vector<Triangle3D> Mesh;



template <typename T>
GLuint initEBO(const T *indices, const int numElements, const bool isDynamic);


void initAttributeVariable(const GLuint &program, const std::string a_attribute, const GLuint buffer, const int numComponents);

template <typename T1, typename T2>
VertexArrayObject initVAO(const GLuint program, const T1 *verts, const T2 *indices, const int numPoints, const int numElements, const bool isDynamic);


template <typename T>
void updateVAO(VertexArrayObject &VAO, const T *vertData, const int numPoints, const int whichVBO);

void initShaderProgram(void);

void render();
void displayMainWindow(void);

void animationMenu(int option);
void createAnimationMenus();
void resetTransformation();
void myKeyboard(unsigned char key, int x, int y);
void myReshape(int w, int h);
void assignGlutFunctions(void(*display)(), void(*keyboad)(unsigned char, int, int) = nullptr, void(*mouse)(int, int, int, int) = nullptr,
	void(*reshape)(int, int) = nullptr, void(*timer)(int) = nullptr, void(*idle)() = nullptr, void(*menuFunc)() = nullptr);

int createOpenGLContext(const char* windowName, const int x, const int y, const int w, const int h);
void initCamera(Camera &camera, bool usePerspective);
void setCamProjection(Camera &camera, int option);
void initScene();

int loadSFM(char* fileName, Mesh &mesh);
#endif