#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Angel.h"

class Camera {
private:
	vec4 position = vec4();
	vec4 target = vec4();
	vec4 up = vec4();

	mat4 viewMatrix = Angel::identity();
	
	/*mat4 projViewMatrix = Angel::identity();*/

	float fovy, aspect, zNear, zFar, left, right, top, bottom;

public:
	mat4 projMatrix = Angel::identity();
	mat4 projViewMatrix = Angel::identity();

	void LookAt(const vec4& eye, const vec4& at, const vec4& up) {		
		this->position = eye;
		this->target = at;
		this->up = up;
	}

	void Ortho(const GLfloat left, const GLfloat right,
		const GLfloat bottom, const GLfloat top,
		const GLfloat zNear, const GLfloat zFar)
	{

		this->projMatrix = Angel::Ortho(left, right, bottom, top, zNear, zFar);

		this->left = left;
		this->right = right;
		this->bottom = bottom;
		this->top = top;
		this->zNear = zNear;
		this->zFar = zFar;
	}

	void Perspective(const GLfloat fovy, const GLfloat aspect,
		const GLfloat zNear, const GLfloat zFar)
	{
		this->projMatrix = Angel::Perspective(fovy, aspect, zNear, zFar);


		this->fovy = fovy;
		this->aspect = aspect;

		this->zNear = zNear;
		this->zFar = zFar;

		this->top = tan(this->fovy * DegreesToRadians / 2) * this->zNear;;
		this->right = this->top * this->aspect;

		//this->bottom = bottom;
		//this->left = top;

	}

	void translate(const float x, const float y, const float z) {		
		vec4 translationVector = vec4(x, y, z, 0);
		this->position += translationVector;
	}

	void updateMatrix() {
		this->viewMatrix = Angel::LookAt(this->position, this->target, this->up);
		projViewMatrix = projMatrix * viewMatrix;
	}


};
#endif