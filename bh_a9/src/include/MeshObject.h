
#ifndef __MESHOBJECT_H__
#define __MESHOBJECT_H__

#include "main.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

typedef vec3 point3;
typedef vec3 color3;


class MeshObject {
public:
	static const int NUM_VERTS = 8;
	static const int NUM_FACES = 12;

	Mesh mesh;
	// Vertices of a unit cube centered at origin
	// sides aligned with axes
	//vector<point3> vertices;
	//vector<point3> normals;
	//vector<point3> faces;

	// RGBA colors of the vertices



	VertexArrayObject vao;

	// vertices color
	//float VertsColors[NUM_VERTS * 3] = { 1.0 };

	// face indices


	// transformation matrices
	mat4 scaleMatrix = Angel::identity();
	mat4 rotationMatrix = Angel::identity();
	mat4 translationMatrix = Angel::identity();
	mat4 compositeMatrix = Angel::identity();

	

	// transformation functions
	void rotate(const float x, const float y, const float z) {
		this->rotationMatrix = RotateX(x)*RotateY(y)*RotateZ(z)*this->rotationMatrix;
	}

	void scale(const float x, const float y, const float z) {
		//vec3 scaleFactor = vec3(x, y, z);
		mat4 scaleFactor = Scale(x, y, z);
		scaleMatrix = scaleFactor*scaleMatrix;
	}

	void translate(const float x, const float y, const float z) {
		vec3 translationVector = vec3(x, y, z);
		translationMatrix = Translate(translationVector)*translationMatrix;
	}

	void setIdentity() {
		scaleMatrix = Angel::identity();
		rotationMatrix = Angel::identity();
		translationMatrix = Angel::identity();
		compositeMatrix = Angel::identity();
	}

	void updateMatrix() {
		compositeMatrix = translationMatrix * rotationMatrix * scaleMatrix;
	}

	void computerNormals() {

	}

	//void init(GLuint program) {
	//	// create vertex array object
	//	vao = initVAO(program, this->vertices, this->faces, NUM_VERTS * 3, NUM_FACES * 3, false);
	//}

};
#endif