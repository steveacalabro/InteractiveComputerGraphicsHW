#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Angel.h"

class Camera {
private:
	vec4 position = vec4();
	vec4 target = vec4();
	vec4 up = vec4();

	vec4 resetPosition = vec4();
	vec4 resetTarget = vec4();
	vec4 resetUp = vec4();


	/*mat4 viewMatrix = Angel::identity();*/
	
	/*mat4 projViewMatrix = Angel::identity();*/

	float fovy, aspect, zNear, zFar, left, right, top, bottom;
	float resetFovy, resetAspect, resetZnear, resetZfar;

	bool defaultViewSet{ false };
	bool defaultProjSet{ false };

public:
	mat4 projMatrix = Angel::identity();
	mat4 viewMatrix = Angel::identity();
	mat4 projViewMatrix = Angel::identity();

	void LookAt(const vec4& eye, const vec4& at, const vec4& up) {		
		this->position = eye;
		this->target = at;
		this->up = up;

		if (!defaultViewSet) {
			setDefaultView(eye,at,up);
			defaultViewSet = true;
		}
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

		this->fovy = fovy;
		this->aspect = aspect;

		this->zNear = zNear;
		this->zFar = zFar;

		this->top = tan(this->fovy * DegreesToRadians / 2) * this->zNear;;
		this->right = this->top * this->aspect;

		//this->bottom = bottom;
		//this->left = top;

		if (!defaultProjSet) {
			setDefaultProj(fovy, aspect, zNear, zFar);
			defaultProjSet = true;
		}

	}

	void translate(const float x, const float y, const float z) {		
		vec4 translationVector = vec4(x, y, z, 0);
		this->position += translationVector;
	}

	void updateMatrix() {
		this->viewMatrix = Angel::LookAt(this->position, this->target, this->up);
		this->projMatrix = Angel::Perspective(this->fovy, this->aspect, this->zNear, this->zFar);
		projViewMatrix = projMatrix * viewMatrix;
	}

	void setDefaultView(const vec4& eye, const vec4& at, const vec4& up) {
		this->resetPosition = eye;
		this->resetTarget = at;
		this->resetUp = up;

	}
	void setDefaultProj(const GLfloat fovy, const GLfloat aspect, const GLfloat zNear, const GLfloat zFar)
	{
		this->resetFovy = fovy;
		this->resetAspect = aspect;
		this->resetZnear = zNear;
		this->resetZfar = zFar;

	}

	void reset() {
		this->position = this->resetPosition;
		this->target = this->resetTarget;
		this->up = this->resetUp;

		this->fovy = resetFovy;
		this->aspect = resetAspect;
		this->zNear = resetZnear;
		this->zFar = resetZfar;

		updateMatrix();
	}
};
#endif