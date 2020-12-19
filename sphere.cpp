#include <iostream>

#include <math.h>

#include "sphere.h"



using namespace std;


//create a 'null' sphere using a blank vector3 as its center point and a radius of Not a Number
sphere::sphere()
{
	center = vector3();
	radius = NAN;
}

//determines whether and intersect occurs with the spehere based on the line descibes by two points in space.
vector3 sphere::intersect(vector3 v1, vector3 v2) {

	float dx = (v2.x - v1.x);
	float dy = (v2.y - v1.y);
	float dz = (v2.z - v1.z);

	float cx = center.x - v1.x;
	float cy = center.y - v1.y;
	float cz = center.z - v1.z;

	float a = (dx * dx) + (dy * dy) + (dz * dz);
	float b = -2 * ((dx * cx) + (dy * cy) + (dz * cz));
	float c = (cx * cx) + (cy * cy) + (cz * cz) - (radius * radius);

	float preRoot = (b * b) - 4 * a * c;

	//if less than 0, does not interesect, return NAN vector
	if (preRoot < 0)
		return vector3();

	float t1 = (-b + sqrt(preRoot)) / (2 * a);
	float t2 = (-b - sqrt(preRoot)) / (2 * a);

	vector3 iPt1 = vector3(v1.x + t1 * dx, v1.y + t1 * dy, v1.z + t1 * dz);
	vector3 iPt2 = vector3(v1.x + t2 * dx, v1.y + t2 * dy, v1.z + t2 * dz);

	if (v1.distance(iPt1) < v1.distance(iPt2))
		return iPt1;
	else
		return iPt2;
}

//determines whether the intersect of the sphere occurs between the two points
vector3 sphere::intersectBetween(vector3 si, vector3 li) {
	float errRate = 0.05;
	vector3 iPt = intersect(si, li);

	float distA = si.distance(iPt);
	float distB = iPt.distance(li);
	float distC = si.distance(li);

	if (((distA + distB) - distC) < errRate)
		return iPt;
	else
		return vector3();
}