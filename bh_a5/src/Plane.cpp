#include "Plane.h"

// Plane class for frustum planes (or any plane...)
Plane::Plane(vec3 normal, point3 point) {
	this->normal = normal != NULL ? normal : vec3(0, 0, 0);
	this->point = point != NULL ? point : point3(0, 0, 0);
	this->d = NULL;
	this->computeParameters();

}

Plane::~Plane() {
	this->normal = NULL;
	this->point = NULL;
	this->d = NULL;
}


// the function set3Points computes the plane's normal and distance to origin using 3 points on the plane
// Note that the order of the points will affect the normal direction of the plane.
void Plane::set3Points(point3 p1, point3 p2, point3 p3) {
	vec3 vec1, vec2;

	vec1 = p1 - p2;
	vec2 = p3 - p2;

	this->normal = cross(vec2, vec1);
	this->point = p2;
	computeParameters();

};

// compute distance between a point and the plane
double Plane::computeDistance(point3 p) {
	return (this->d + dot(this->normal, p));
};


void Plane::computeParameters() {
	//normalize the normal in case the input normal is not normalized
	this->normal = normalize(this->normal);

	// the distance between the plane and the world origin
	this->d = -(dot(this->normal, this->point));
};



// Ray class
//function Ray(dir, startPoint) {
//
//	// the point should be the start point instead of any point on the ray!
//	this->direction = dir != = undefined ? dir : new Vector3(0, 0, 0);
//	this->point = startPoint != = undefined ? startPoint : new Vector3(0, 0, 0);
//}
//
//
//// ray: P = P0 + t*v
//// Plane: P*N +d = 0
//// solve this formula we get: t = -(p0*N+d)/(v*N)
//Ray.prototype.intersectPlane = function(plane) {
//	if (plane.type != 'Plane') {
//		console.log('The input is not a plane!');
//		return
//	}
//	var p0 = this->point,
//		N = plane.normal,
//		d = plane.d,
//		v = new Vector3(this->direction.elements); // use new in case the multiplyScalar change the value of this->direction
//												  //v = this->direction;
//
//	var t = -(dotProd(p0, N) + d) / dotProd(v, N);
//	var intersection = add(p0, v.multiplyScalar(t));
//	return intersection
//};