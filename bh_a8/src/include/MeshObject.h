
#ifndef __MESHOBJECT_H__
#define __MESHOBJECT_H__

#include "main.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

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


struct VertexArrayObject
{
	GLuint id;
	GLuint vertVBO;
	//GLuint colorVBO;
	GLuint EBO;
};

enum ObjectColorIndex
{
	ORIGINAL, PICKED
};

class MeshObject {
public:

	Mesh mesh;

	VertexArrayObject vao;

	color3 objectColor[2];
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


};
#endif