#ifndef __BOXGEOMETRY_H__
#define __BOXGEOMETRY_H__

#include "main.h"

typedef vec4 point4;
typedef vec4 color4;


class BoxGeometry {
public:
	static const int NUM_VERTS = 8;
	static const int NUM_FACES = 12;


	// Vertices of a unit cube centered at origin
	// sides aligned with axes
	point4 vertices[NUM_VERTS] = {
		point4(-0.5, -0.5, 0.5, 1.0),
		point4(-0.5, 0.5, 0.5, 1.0),
		point4(0.5, 0.5, 0.5, 1.0),
		point4(0.5, -0.5, 0.5, 1.0),
		point4(-0.5, -0.5, -0.5, 1.0),
		point4(-0.5, 0.5, -0.5, 1.0),
		point4(0.5, 0.5, -0.5, 1.0),
		point4(0.5, -0.5, -0.5, 1.0)
	};

	// RGBA colors of the vertices
	color4 vertex_colors[NUM_VERTS] = {
		color4(0.0, 0.0, 0.0, 1.0), // black
		color4(1.0, 0.0, 0.0, 1.0), // red
		color4(1.0, 1.0, 0.0, 1.0), // yellow
		color4(0.0, 1.0, 0.0, 1.0), // green
		color4(0.0, 0.0, 1.0, 1.0), // blue
		color4(1.0, 0.0, 1.0, 1.0), // magenta
		color4(1.0, 1.0, 1.0, 1.0), // white
		color4(0.0, 1.0, 1.0, 1.0) // cyan
	};

	/*float Vertices[NUM_VERTS * 3] = { 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5,
		0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5,
		0.5, 0.5, 0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5, 0.5, 0.5,
		-0.5, 0.5, 0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5,
		-0.5, -0.5, -0.5, 0.5, -0.5, -0.5, 0.5, -0.5, 0.5, -0.5, -0.5, 0.5,
		0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5 };*/

	VertexArrayObject vao;

	// vertices color
	//float VertsColors[NUM_VERTS * 3] = { 1.0 };

	// face indices
	int faces[NUM_FACES * 3] = { 0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7,
		8, 9, 10, 8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23 };



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

	void init(GLuint program) {
		// create vertex array object
		vao = initVAO(program, this->vertices, this->faces, NUM_VERTS * 3, NUM_FACES * 3, false);
	}

};
#endif