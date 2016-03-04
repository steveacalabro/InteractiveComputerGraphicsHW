#ifndef __SCENE_H__
#define __SCENE_H__

#include "Angel.h"

class Scene {
public:
	mat4 scaleMatrix = Angel::identity();
	mat4 rotationMatrix = Angel::identity();
	mat4 translationMatrix = Angel::identity();
	mat4 compositeMatrix = Angel::identity();

	void rotate(const float x, const float y, const float z) {
		//rotationMatrix = RotateX(x)*rotationMatrix;
		//rotationMatrix = RotateY(y)*rotationMatrix;
		//rotationMatrix = RotateZ(z)*rotationMatrix;

		this->rotationMatrix = RotateX(x)*RotateY(y)*RotateZ(z)*this->rotationMatrix;

	}

	void scale(const float x, const float y, const float z) {
		vec3 scaleFactor = vec3(x, y, z);
		this->scaleMatrix = Scale(scaleFactor)*this->scaleMatrix;
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