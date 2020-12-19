#include <iostream>

#include <math.h>

#include "plane.h"



using namespace std;


//default constructor which creates the vector obect with an x,y,z position that are Not A Number. This can be used by the program to determine whether an object is 'null' and not relevant in its calculations
plane::plane()
{
	a = NAN;
	b = NAN;
	c = NAN;
	d = NAN;
}

vector3 plane::intersect(vector3 v1, vector3 v2) {

	// find parametric form

	float dx = v1.x;

	float dy = v1.y;

	float dz = v1.z;



	float tx = (v2.x - v1.x);

	float ty = (v2.y - v1.y);

	float tz = (v2.z - v1.z);



	// resolve t

	float constant = (a * dx) + (b * dy) + (c * dz);

	float tcoef = (a * tx) + (b * ty) + (c * tz);

	float t = (-d - constant) / tcoef;



	// resolve

	float x_int = dx + (tx * t);

	float y_int = dy + (ty * t);

	float z_int = dz + (tz * t);



	return vector3(x_int, y_int, z_int);

}

//determines whether the intersect of the plane occurs between the two points si and li.
vector3 plane::intersectBetween(vector3 si, vector3 li) {
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

//determines whether the intersect of the plane occurs within the bounds of the four corners of the plane
bool plane::boundaryBool(vector3 iPt)
{
	float errRate = 0.05;

	if (iPt.x >= (minX - errRate) && iPt.x <= (maxX + errRate)) {
		if (iPt.y >= (minY - errRate) && iPt.y <= (maxY + errRate)) {
			if (iPt.z >= (minZ - errRate) && iPt.z <= (maxZ + errRate)) {
				return true;
			}
		}
	}
	return false;
}


//Set methods have functionality to set the limits of the plane boundary

void plane::setP1(vector3 v) {

	p1x = v.x; p1y = v.y; p1z = v.z;

	setLimits(p1x, p1y, p1z);

}



void plane::setP2(vector3 v) {

	p2x = v.x; p2y = v.y; p2z = v.z;

	setLimits(p2x, p2y, p2z);
}



void plane::setP3(vector3 v) {

	p3x = v.x; p3y = v.y; p3z = v.z;


	setLimits(p3x, p3y, p3z);
}



void plane::setP4(vector3 v) {

	p4x = v.x; p4y = v.y; p4z = v.z;

	setLimits(p4x, p4y, p4z);

}



vector3 plane::getP1() {

	return vector3(p1x, p1y, p1z);

}



vector3 plane::getP2() {

	return vector3(p2x, p2y, p2z);

}



vector3 plane::getP3() {

	return vector3(p3x, p3y, p3z);

}



vector3 plane::getP4() {

	return vector3(p4x, p4y, p4z);

}

//sets the boundaries of the plane using the minimum and maximum x/y/z positions of the four corner points.
void plane::setLimits(float x, float y, float z) {
	if (!limitsSet) {
		minX = x;
		minY = y;
		minZ = z;

		maxX = x;
		maxY = y;
		maxZ = z;

		limitsSet = true;
	}
	else {
		if (x < minX)
			minX = x;
		if (x > maxX)
			maxX = x;

		if (y < minY)
			minY = y;
		if (y > maxY)
			maxY = y;

		if (z < minZ)
			minZ = z;
		if (z > maxZ)
			maxZ = z;
	}
}