#ifndef __PLANE_H__
#define __PLANE_H__

#include "main.h"


typedef vec3 point3;
typedef vec3 color3;
typedef vec4 point4;
typedef vec4 color4;


class Plane {
public:
	Plane(vec3 normal = NULL, point3 point = NULL);
	~Plane();

	vec3 normal;
	point3 point;
	double d;
	void computeParameters();
	void set3Points(point3 p1, point3 p2, point3 p3);
	double computeDistance(point3 p);


};







#endif