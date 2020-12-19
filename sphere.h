#ifndef _VECTOR3
#define _VECTOR3
#include "vector3.h"
#endif


//Descibes a spehere object and includes methods to determine intersects.
class sphere {

public:

	// constructors

	sphere();

	sphere(vector3 c, float r) : center(c), radius(r) {};



	// methods

	vector3 intersect(vector3 v1, vector3 v2);

	vector3 intersectBetween(vector3 si, vector3 li);


	// data elements

	vector3 center;

	float radius;


};
