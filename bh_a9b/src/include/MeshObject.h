
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
typedef vec4 point4;
typedef vec4 color4;

// this is color3 byte version, color3 is float
struct color3Byte
{
	GLubyte  x;
	GLubyte  y;
	GLubyte  z;

	//
	//  --- Constructors and Destructors ---
	//

	color3Byte(GLubyte s = GLubyte(0)) :
		x(s), y(s), z(s) {}

	color3Byte(GLubyte x, GLubyte y, GLubyte z) :
		x(x), y(y), z(z) {}

	color3Byte(const color3Byte& v) { x = v.x;  y = v.y;  z = v.z; }

	//
	//  --- Indexing Operator ---
	//
	GLubyte& operator [] (int i) { return *(&x + i); }
	const GLubyte operator [] (int i) const { return *(&x + i); }
};

typedef vector<vector<color3Byte>> Image;

// 3D triangle, each triangle has 3 vertices and 3 normals
struct Triangle3D
{
	int faces[3]; // face id in SMF model
	vec3 vertices[3]; // 3 vertex coord
	vec3 normals[3]; // 3 normal 
	vec3 triangleNormal; // the normal of the triangle plane
	vec3 centerOfMass;
	vec3 texCoord[3];
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
	ORIGINAL, PICKED, RENDER
};

struct Material {
	// absobtion coefficients
	color4 kd;
	color4 ks;
	color4 ka;
	float shininess;
};

class MeshObject {
public:

	Mesh mesh;

	VertexArrayObject vao;
	VertexArrayObject pickingVao;

	color3 objectColor[3];
	Material material;
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