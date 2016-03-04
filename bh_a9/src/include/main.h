#ifndef __MAIN_H__
#define __MAIN_H__

#include "Angel.h"
//#include "BoxGeometry.h"
#include "Camera.h"
#include "Scene.h"
#include <vector>
#include <iostream>
#include "MeshObject.h"

enum class VBO_OPTION;
using namespace std;

struct IndexBufferObject
{
	GLuint id;
	GLuint numIndices;
};

struct Program
{
	GLuint lineShader;
	GLuint polygonShader;
};

template <typename T>
GLuint initEBO(const T *indices, const int numElements, const bool isDynamic);


void initAttributeVariable(const GLuint &program, const std::string a_attribute, const GLuint buffer, const int numComponents);

template <typename T1, typename T2>
VertexArrayObject initVAO(const GLuint program, const T1 *verts, const T2 *indices, const int numPoints, const int numElements, const bool isDynamic);


template <typename T>
void updateVAO(VertexArrayObject &VAO, const T *vertData, const int numPoints, VBO_OPTION whichVBO);

void initShaderProgram(void);

void render();
void selectControlPoint(int selectedPoint);
void resetControlPoint(int selectedPoint);
void initControlPoints();
void displayMainWindow(void);
void initMesh();

void projectionMenu(int option);
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

void tessellate(const vector<vector<point3>> &patchPoints, vector<point3> &vertices, vector<point3> &faces, vector<vec3> &normals, vector<vec2> &texCoord);
int loadSFM(char* fileName, Mesh &mesh);
int loadBezir(char* fileName, Mesh &mesh, vector<point3> &controlPoints);
vector<vector<point3>> interpolateBezirPatch(const vector<point3> &controlPoints, const int &resolution);
void constructMesh(const vector<point3> &controlPoints, int resolution, Mesh &mesh);
void exportSMF(string fileName, const vector<point3> &vertices, const vector<point3> &faces, const vector<point3> &normals);
void initTexture();
#endif