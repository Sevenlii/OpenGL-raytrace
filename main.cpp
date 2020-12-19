/* 
   The program includes an array of planes objects which stores 6 planes objects that acts as the sides of a cube and a single plane for the floor. An array of sphere objects is used to store a new sphere class.
   Lastly a vector3 array for lights is used to light positions.
   After creating the 6 planes and a single sphere object, and their respective locations and surface properties, as well as the lighting conditions, the program sends rays through the scene based on a viewer position.
   Each ray looks for intersections with any plane or spehere object on its path, and determines which one is closest to the viewer. A reflected ray is then considered (up to a max depth) to compliment the original intersect surface colour.
   Shadows are also calculated based on whether the surface intersected is directly visible from the lighting positions. The more lighting positions that are not visible from the surface, the darker the shadow, up to a maximum abient condition.
   The program renders an animation of the sphere orbiting the cube, updating shadows accordingly.

   functions
   ---
   setLights - sets up lighting conditions and positions for the scene
   setSpheres - sets the surface values and instanitates sphere objects for the various planes in the scene
   setSurfaces - sets the surface values and instanitates plane objects for the various planes in the scene
   inFront - returns a boolean value representing if a point in space is in the direction of travel from a second point in space.
   checkerboard - returns a vector3 representing the colour (either black or white) of a surface based on its x/y/z position. This is used to create the checkerboard effect of the floor surface
   localColor - determines the color of a surface at a point based on the lighting
   rayTrace - based on an origin point and vector, determines the color a pixel on the viewplane should be by recursively calling rayTrace and determining a color of intersected objects
   render - for each pixel point on the viewplane, sends a ray into the scene to determine what colour the pixel should be, draws pixel to screen
   setViewPlane - calculates the view plane based on viewer position, look at, fov
   initialize - sets background color
   display - calls to set view play and render scene
   reshape - called on window reshape
   updateSpehere - updates the position of the spehere for the animation effect
   main - main loop runs program

*/


#include <iostream>
#include <math.h>
#include "gl/glut.h"
#include "plane.h"
#include "sphere.h"



using namespace std;
//pi
const GLfloat PI = 3.14159265;
//max raytracing depth
const GLint MAX_RAYTRACE_DEPTH = 3;

// viewer
int dStep = 0;
vector3 viewer(130 * cos(dStep * 36 * PI / 180.0), 200.0, 130.0 * sin(dStep * 36 * PI / 180));

//view plane resolution dimensions
const int height = 500;
const int width = 500;

//sphere orbit
int orbit = 90;
float orbitMaxX = 50;
float orbitMaxZ = 50;
float orbitCenterX = 0.0;
float orbitCenterY = 15.0;
float orbitCenterZ = 0.0;

//cube location
float cubeCenterX = 0.0;
float cubeCenterY = 30.0;
float cubeCenterZ = 0.0;
float cubeWidth = 50.0;
float cubeHeight = 50.0;
float cubeDepth = 50.0;

/*	view plane params

	change look at and up direction to change point of view

	normal and plane will be changed during execution
*/
vector3 viewPlaneLookAt(0, 0, 0);
vector3 viewPlaneUp(0.0, 1.0, 0.0);

vector3 viewPlaneNormal(0.0, 0.0, 0.0);
plane viewPlane(0.0, 0.0, 0.0, 0.0);

/*	FOV

	Field of view and aspect ratio will determine how much of the scene is visible
*/
GLfloat ver_FOV = 90;
GLfloat aspectRatio = 1;

/*	set up frame buffer

	used to store pixel information
	500 data points of r, g, b, a
*/
GLfloat frameBuffer[height * width * 4];

/* plane and sphere surface storage

*/
int numPlanes;
plane planeArray[99];
vector3 planeColours[99];

int numSpheres;
sphere sphereArray[99];
vector3 sphereColours[99];

/*	reflective coeficients of each surface

*/
float planeReflectCoef[99];
float sphereReflectCoef[99];

// colors

vector3 grey(0.75, 0.75, 0.75);

vector3 lightRed(0.75, 0.1, 0.1);

vector3 blue(0.1, 0.1, 0.9);

vector3 noColour(-1.0, -1.0, -1.0);

// lighting position
int numLights;
vector3 lights[99];


// lighting elements

float ambient_coef = 0.3;

float diffuse_coef = 0.7;

float reflect_coef = 0.4;

float local_coef = 0.6;

//create lights
void setLights() {
	//create lights with vector3 positions, store in light array
	int i = 0;
	lights[i] = vector3(5, 50, 100.0);
	i++;
	lights[i] = vector3(-75, 75, 55.0);
	i++;
	//set number of lights to global varibale
	numLights = i;
}

//create spshere objects
void setSpheres() {
	//create base variables
	vector3 center(0, 0, 0);
	float radius = 0;
	int i = 0;

	//set orbiting sphere
	radius = 4.0;
	float orbitDeg = (float)orbit / 180 * PI;
	float x = orbitMaxX * cos(orbitDeg);
	float z = (float)orbitMaxZ * sin(orbitDeg);
	center = vector3(x + orbitCenterX, orbitCenterY, z + orbitCenterZ);
	//add to array
	sphereArray[i] = sphere(center, radius);
	sphereColours[i] = blue;
	sphereReflectCoef[i] = 0.0;
	//increment number of spehere
	i++;
	
	//set stationary sphere
	radius = 4.0;
	center = vector3(cubeCenterX, (cubeCenterY + cubeHeight / 2) + radius + 3, cubeCenterZ);
	sphereArray[i] = sphere(center, radius);
	sphereColours[i] = blue;
	sphereReflectCoef[i] = 0.0;
	i++;
	
	//set sphere number global varibale
	numSpheres = i;
}

//creates surfaces for object in the scene
void setSurfaces() {
	//create point reference
	vector3 point(0, 0, 0);
	//array increment number
	int i = 0;

	//floor plane
	planeArray[i] = plane(0.0, 1.0, 0.0, -4.0);
	point = vector3(-75.0, 4.0, 75.0);
	planeArray[i].setP1(point);
	point = vector3(-75.0, 4.0, -75.0);
	planeArray[i].setP2(point);
	point = vector3(75.0, 4.0, -75.0);
	planeArray[i].setP3(point);
	point = vector3(75.0, 4.0, 75.0);
	planeArray[i].setP4(point);

	planeColours[i] = noColour;
	planeReflectCoef[i] = 0.0;
	i++;

	//cube face 1 - front
	planeArray[i] = plane(0.0, 0.0, 1.0, -(cubeCenterZ + cubeDepth / 2));
	point = vector3((cubeCenterX - cubeWidth / 2), (cubeCenterY - cubeHeight / 2), (cubeCenterZ + cubeDepth / 2));
	planeArray[i].setP1(point);
	point = vector3((cubeCenterX - cubeWidth / 2), (cubeCenterY + cubeHeight / 2), (cubeCenterZ + cubeDepth / 2));
	planeArray[i].setP2(point);
	point = vector3((cubeCenterX + cubeWidth / 2), (cubeCenterY + cubeHeight / 2), (cubeCenterZ + cubeDepth / 2));
	planeArray[i].setP3(point);
	point = vector3((cubeCenterX + cubeWidth / 2), (cubeCenterY - cubeHeight / 2), (cubeCenterZ + cubeDepth / 2));
	planeArray[i].setP4(point);

	planeColours[i] = lightRed;
	planeReflectCoef[i] = 0.4;
	i++;

	//cube face 1 - back
	planeArray[i] = plane(0.0, 0.0, 1.0, -(cubeCenterZ - cubeDepth / 2));
	point = vector3((cubeCenterX + cubeWidth / 2), (cubeCenterY - cubeHeight / 2), (cubeCenterZ - cubeDepth / 2));
	planeArray[i].setP1(point);
	point = vector3((cubeCenterX + cubeWidth / 2), (cubeCenterY + cubeHeight / 2), (cubeCenterZ - cubeDepth / 2));
	planeArray[i].setP2(point);
	point = vector3((cubeCenterX - cubeWidth / 2), (cubeCenterY + cubeHeight / 2), (cubeCenterZ - cubeDepth / 2));
	planeArray[i].setP3(point);
	point = vector3((cubeCenterX - cubeWidth / 2), (cubeCenterY - cubeHeight / 2), (cubeCenterZ - cubeDepth / 2));
	planeArray[i].setP4(point);

	planeColours[i] = lightRed;
	planeReflectCoef[i] = 0.4;
	i++;

	//cube face 3 - left
	planeArray[i] = plane(1.0, 0.0, 0.0, -(cubeCenterX - cubeWidth / 2));
	point = vector3((cubeCenterX - cubeWidth / 2), (cubeCenterY - cubeHeight / 2), (cubeCenterZ - cubeDepth / 2));
	planeArray[i].setP1(point);
	point = vector3((cubeCenterX - cubeWidth / 2), (cubeCenterY + cubeHeight / 2), (cubeCenterZ - cubeDepth / 2));
	planeArray[i].setP2(point);
	point = vector3((cubeCenterX - cubeWidth / 2), (cubeCenterY + cubeHeight / 2), (cubeCenterZ + cubeDepth / 2));
	planeArray[i].setP3(point);
	point = vector3((cubeCenterX - cubeWidth / 2), (cubeCenterY - cubeHeight / 2), (cubeCenterZ + cubeDepth / 2));
	planeArray[i].setP4(point);

	planeColours[i] = lightRed;
	planeReflectCoef[i] = 0.4;
	i++;

	//cube face 4 - right
	planeArray[i] = plane(1.0, 0.0, 0.0, -(cubeCenterX + cubeWidth / 2));
	point = vector3((cubeCenterX + cubeWidth / 2), (cubeCenterY - cubeHeight / 2), (cubeCenterZ + cubeDepth / 2));
	planeArray[i].setP1(point);
	point = vector3((cubeCenterX + cubeWidth / 2), (cubeCenterY + cubeHeight / 2), (cubeCenterZ + cubeDepth / 2));
	planeArray[i].setP2(point);
	point = vector3((cubeCenterX + cubeWidth / 2), (cubeCenterY + cubeHeight / 2), (cubeCenterZ - cubeDepth / 2));
	planeArray[i].setP3(point);
	point = vector3((cubeCenterX + cubeWidth / 2), (cubeCenterY - cubeHeight / 2), (cubeCenterZ - cubeDepth / 2));
	planeArray[i].setP4(point);

	planeColours[i] = lightRed;
	planeReflectCoef[i] = 0.4;
	i++;
	
	//cube face 5 - top
	planeArray[i] = plane(0.0, 1.0, 0.0, -(cubeCenterY + cubeHeight / 2));
	point = vector3((cubeCenterX - cubeWidth / 2), (cubeCenterY + cubeHeight / 2), (cubeCenterZ + cubeDepth / 2));
	planeArray[i].setP1(point);
	point = vector3((cubeCenterX - cubeWidth / 2), (cubeCenterY + cubeHeight / 2), (cubeCenterZ - cubeDepth / 2));
	planeArray[i].setP2(point);
	point = vector3((cubeCenterX + cubeWidth / 2), (cubeCenterY + cubeHeight / 2), (cubeCenterZ - cubeDepth / 2));
	planeArray[i].setP3(point);
	point = vector3((cubeCenterX + cubeWidth / 2), (cubeCenterY + cubeHeight / 2), (cubeCenterZ + cubeDepth / 2));
	planeArray[i].setP4(point);

	planeColours[i] = lightRed;
	planeReflectCoef[i] = 0.4;
	i++;

	/*	DISABLED
	//cube face 6 - bottom
	planeArray[i] = plane(0.0, 1.0, 0.0, -(cubeCenterY - cubeHeight / 2));
	point = vector3((cubeCenterX - cubeWidth / 2), (cubeCenterY - cubeHeight / 2), (cubeCenterZ - cubeDepth / 2));
	planeArray[i].setP1(point);
	point = vector3((cubeCenterX - cubeWidth / 2), (cubeCenterY - cubeHeight / 2), (cubeCenterZ + cubeDepth / 2));
	planeArray[i].setP2(point);
	point = vector3((cubeCenterX + cubeWidth / 2), (cubeCenterY - cubeHeight / 2), (cubeCenterZ + cubeDepth / 2));
	planeArray[i].setP3(point);
	point = vector3((cubeCenterX + cubeWidth / 2), (cubeCenterY - cubeHeight / 2), (cubeCenterZ - cubeDepth / 2));
	planeArray[i].setP4(point);

	planeColours[i] = lightRed;
	planeReflectCoef[i] = 0.4;
	i++;
	*/
	numPlanes = i;
}




//returns true if destination is in the direction of travel from the origin
bool inFront(vector3 originPt, vector3 destPt, vector3 dirRay) {
	bool front = false;

	//absolute distance from origin to destination
	float oriDist = originPt.distance(destPt);

	//absolute distance from origin plus normalized direction
	vector3 dirPt = originPt.add(dirRay.normalize());
	float dirDist = dirPt.distance(destPt);

	//if destination is in the direction of travel, the distance from the destination to the
	//direction point should be smaller than the distance to the origin
	if (dirDist < oriDist) {
		front = true;
	}

	return front;
}

//creates a checkerboard pattern on a surface determined by x and z coordinates.
vector3 checkerboard(vector3 intersect, vector3 normal) {
	vector3 baseColor(1.0, 1.0, 1.0);

	GLint squareSize = 20;
	GLint cbSquare = ((int)intersect.x / squareSize);

	//offset the checkbaord for negative numbers
	if (intersect.x < 0)
		cbSquare -= 1;

	if (cbSquare % 2 == 0)
		baseColor = vector3(0.0, 0.0, 0.0);

	cbSquare = ((int)intersect.z / squareSize);
	if (intersect.z < 0)
		cbSquare -= 1;

	if (cbSquare % 2 == 0) {
		baseColor.x = 1.0 - baseColor.x;
		baseColor.y = 1.0 - baseColor.y;
		baseColor.z = 1.0 - baseColor.z;
	}

	//for each light

	float totalLightContr = 0;

	for (int i = 0; i < numLights; i++) {

		// calculate unit vector

		vector3 origin = lights[i].subtract(intersect);

		vector3 unitVec = origin.normalize();



		// calculate dot product

		float dotProd = unitVec.dot(normal);

		//calc intensity due to diffusion

		totalLightContr += diffuse_coef * dotProd;

	}

	// calculate intensity

	float ambientContr = ambient_coef;

	float intensity = ambientContr + totalLightContr;



	if (intensity > 1.0) intensity = 1.0;

	float r = intensity * baseColor.x;

	float g = intensity * baseColor.y;

	float b = intensity * baseColor.z;

	return vector3(r, g, b);
}

// calculate local color
// local color is intensity * base color

vector3 localColor(vector3 intersect, vector3 baseColor, vector3 normal) {

	//for each light

	float totalLightContr = 0;

	for (int i = 0; i < numLights; i++) {

		// calculate unit vector

		vector3 origin = lights[i].subtract(intersect);

		vector3 unitVec = origin.normalize();



		// calculate dot product

		float dotProd = unitVec.dot(normal);

		//calc intensity due to diffusion

		totalLightContr += diffuse_coef * dotProd;

	}

	if (totalLightContr < 0)
		totalLightContr = 0;

	// calculate intensity

	float ambientContr = ambient_coef;

	float intensity = ambientContr + totalLightContr;



	if (intensity > 1.0) intensity = 1.0;

	float r = intensity * baseColor.x;

	float g = intensity * baseColor.y;

	float b = intensity * baseColor.z;

	return vector3(r, g, b);
}

//returns a colour vector that results from a recursive ray tracing
//if no contribution, returns {-1.0, -1.0, -1.0} vector
vector3 rayTrace(vector3 originPt, vector3 inRay, GLint depth) {
	//init return colour and assume return colour has no contribution, -1.0 vector
	vector3 returnColour(-1.0, -1.0, -1.0);

	//add 1 to depth int
	depth++;

	//if maximum calls return no contribution
	if (depth > MAX_RAYTRACE_DEPTH) {
		return returnColour;
	}
	//otherwise continue calculating colour
	else {
		vector3 surfaceNormal;
		vector3 checkIntersect;
		int surfaceType = -1;
		int checkDist = -1;
		int dist = -1;
		int index = -1;
		//for each plane surface
		for (int i = 0; i < numPlanes; i++) {
			//check if ray intersects surface
			checkIntersect = planeArray[i].intersect(originPt, originPt.add(inRay));
			//only coontinue if intersect is in the direction of the ray
			if (inFront(originPt, checkIntersect, inRay)) {
				//check if ray intersects in the boundary
				if (planeArray[i].boundaryBool(checkIntersect)) {
					//get the distance
					checkDist = originPt.distance(checkIntersect);
					//if it intersects, check if it is closer than the previous found intersect
					if ((dist == -1 || checkDist < dist) && checkDist > 0.1) {
						index = i;
						dist = checkDist;
						surfaceType = 0;
					}
				}
			}
		}

		//for each sphere surface
		for (int i = 0; i < numSpheres; i++) {
			//check if ray intersects surface
			checkIntersect = sphereArray[i].intersect(originPt, originPt.add(inRay));
			//only coontinue if intersect is in the direction of the ray
			if (inFront(originPt, checkIntersect, inRay)) {
				//get the distance
				checkDist = originPt.distance(checkIntersect);
				//if it intersects, check if it is closer than the previous found intersect
				if ((dist == -1 || checkDist < dist) && checkDist > 0.1) {
					index = i;
					dist = checkDist;
					surfaceType = 1;
				}
			}
		}

		//if no object was intersected by ray, index will be -1. return no contribution
		if (index == -1) {
			return returnColour;
		}
		vector3 surfaceIntersect;
		//otherwise, we have index of closest surface, set base color
		if (surfaceType == 0) {
			surfaceNormal = vector3(planeArray[index].a, planeArray[index].b, planeArray[index].c);
			surfaceIntersect = planeArray[index].intersect(originPt, originPt.add(inRay));
		}
		else if (surfaceType == 1) {
			surfaceIntersect = sphereArray[index].intersect(originPt, originPt.add(inRay));
			surfaceNormal = surfaceIntersect.subtract(sphereArray[index].center).normalize();
		}

		surfaceNormal = surfaceNormal.normalize();

		if (surfaceType == 0) {
			//if colour is negative, get checkerboard colour, else get normal local colour
			if (planeColours[index].x < 0.0) {
				returnColour = checkerboard(surfaceIntersect, surfaceNormal);
			}
			else {
				returnColour = localColor(surfaceIntersect, planeColours[index], surfaceNormal);
			}
		}
		else if (surfaceType == 1) {
			//if colour is negative, get checkerboard colour, else get normal local colour
			if (sphereColours[index].x < 0.0) {
				int a = 0;
			}
			else {
				returnColour = localColor(surfaceIntersect, sphereColours[index], surfaceNormal);
			}
		}


		//for each light source, check if an object is obstructing
		int shadowCount = 0;
		int lightDist = -1;
		vector3 lightRay;
		bool nextLight = false;
		for (int j = 0; j < numLights; j++) {
			nextLight = false;
			for (int i = 0; i < numPlanes; i++) {
				//dont check current surface
				if (i != index || surfaceType != 0) {
					//check if the plane is in between the light and the intersect
					checkIntersect = planeArray[i].intersectBetween(surfaceIntersect, lights[j]);
					//if it intersects another object, view is obstructed, no need to continue scaning intersection for this light
					if (planeArray[i].boundaryBool(checkIntersect)) {
						shadowCount++;
						nextLight = true;
						break;
					}
				}
			}
			if (!nextLight) {
				for (int i = 0; i < numSpheres; i++) {
					//dont check current surface
					if (i != index || surfaceType != 1) {
						//check if theplane is in between the light and the intersect
						checkIntersect = sphereArray[i].intersectBetween(surfaceIntersect, lights[j]);
						//if it intersects another object, view is obstructed, no need to continue scaning intersection for this light
						if (!isnan(checkIntersect.x)) {
							shadowCount++;
							nextLight = true;
							break;
						}
					}
				}
			}
		}

		//calc shadow effect, linear scale between ambient and 1.0
		float shadowContr = ((-1.0 + ambient_coef) / numLights) * shadowCount + 1.0;
		returnColour = returnColour.scalar(shadowContr);

		//check reflect
		float reflectCoef = 0.0;
		if (surfaceType == 0)
			reflectCoef = planeReflectCoef[index];
		else if (surfaceType == 0) {
			reflectCoef = sphereReflectCoef[index];
		}

		if (reflectCoef > 0) {
			vector3 addedColour(-1.0, -1.0, -1.0);

			//get reflect angle
			vector3 reflectVector = surfaceIntersect.subtract(originPt).reflect(surfaceNormal).normalize();

			//ray trace the reflect angle
			addedColour = rayTrace(surfaceIntersect, reflectVector, depth);
			if (addedColour.x != -1.0) {
				returnColour = returnColour.scalar(local_coef).add(addedColour.scalar(reflectCoef));
			}
		}

		return returnColour;
	}
}

// render
/*
	based on the view plane, a ray will be cast from the viewer position to a section of the view plane.
	each ray calls to function rayTrace which will determine the colour of pixel that ray would cast on the view plane
	the pixel information is stored in the frameBuffer
	the frame buffer is then draw on the screen
*/
void render() {
	//retrieve background color
	GLfloat bgColor[4];
	glGetFloatv(GL_COLOR_CLEAR_VALUE, bgColor);

	//set color vector to non-colour
	vector3 color(-1.0, -1.0, -1.0);

	//set up view plane intersect pt as bottom left of view plane
	vector3 viewPlaneIntersect = viewPlane.getP1();

	//find horizontal view plane vector
	vector3 hor_dir = viewPlane.getP4().subtract(viewPlane.getP1());
	GLfloat hor_dist = viewPlane.getP1().distance(viewPlane.getP4());

	//find vertial view plane vector
	vector3 ver_dir = viewPlane.getP2().subtract(viewPlane.getP1());
	GLfloat ver_dist = viewPlane.getP1().distance(viewPlane.getP2());

	//change in x,y,z for each increment in height
	vector3 jDelta(hor_dir.x / (width - 1), hor_dir.y / (width - 1), hor_dir.z / (width - 1));

	//increment in width
	vector3 iDelta(ver_dir.x / (height - 1), ver_dir.y / (height - 1), ver_dir.z / (height - 1));

	//for each pixel in height
	for (int i = 0; i < height; i++) {
		//reset view plane intersect to bottom left corner
		viewPlaneIntersect = viewPlane.getP1();
		//add i delta i times
		for (int k = 0; k < i; k++)
			viewPlaneIntersect = viewPlaneIntersect.add(iDelta);
		//for each pixel in width
		for (int j = 0; j < width; j++) {
			color = rayTrace(viewer, viewPlaneIntersect.subtract(viewer).normalize(), 0);
			//calc buffer pointer
			int pointer = 4 * j + 4 * height * i;
			//if color is non-colour set frame point as background
			if (color.x < 0) {
				frameBuffer[pointer] = bgColor[0];
				frameBuffer[pointer + 1] = bgColor[1];
				frameBuffer[pointer + 2] = bgColor[2];
				frameBuffer[pointer + 3] = bgColor[3];
			}
			else {
				frameBuffer[pointer] = color.x;
				frameBuffer[pointer + 1] = color.y;
				frameBuffer[pointer + 2] = color.z;
				frameBuffer[pointer + 3] = 1;
			}
			viewPlaneIntersect = viewPlaneIntersect.add(jDelta);
		}

	}

	//draw frame buffer to screen
	glDrawPixels(500, 500, GL_RGBA, GL_FLOAT, frameBuffer);

}

//sets up viewplane
void setViewPlane() {
	//create view plane normal
	viewPlaneNormal = viewer.subtract(viewPlaneLookAt).normalize();

	//using normal and look at position (point on plane), find equation of the plane
	GLfloat a = viewPlaneNormal.x;
	GLfloat b = viewPlaneNormal.y;
	GLfloat c = viewPlaneNormal.z;
	GLfloat d = viewPlaneNormal.x * -1 * viewPlaneLookAt.x + viewPlaneNormal.y * -1 * viewPlaneLookAt.y + viewPlaneNormal.z * -1 * viewPlaneLookAt.z;

	viewPlane = plane(a, b, c, d);

	//using up vector and viewer normal (negative view plane normal), calc right vector
	vector3 viewPlaneRight = viewPlaneNormal.scalar(-1).cross(viewPlaneUp);

	//height and width of plane (world coordinates)
	float planeHeight = 2 * tan((ver_FOV * PI) / 180 / 2) * viewer.distance(viewPlaneLookAt);
	float planeWidth = planeHeight * aspectRatio;

	//find corners of plane
	vector3 deltaH = viewPlaneUp.scalar(planeHeight / 2);
	vector3 deltaW = viewPlaneRight.scalar(planeWidth / 2);

	//lower left
	vector3 point = viewPlaneLookAt.subtract(deltaH).subtract(deltaW);
	viewPlane.setP1(point);

	//upper left
	point = viewPlaneLookAt.add(deltaH).subtract(deltaW);
	viewPlane.setP2(point);

	//upper right
	point = viewPlaneLookAt.add(deltaH).add(deltaW);
	viewPlane.setP3(point);

	//lower right
	point = viewPlaneLookAt.subtract(deltaH).add(deltaW);
	viewPlane.setP4(point);
}

// initialize

void initialize() {

	// set background color

	glClearColor(0.5, 0.7, 0.5, 0.0);

	//set lights
	setLights();

	//set up spheres
	setSpheres();

	//set up square planes in scene
	setSurfaces();

	//call to set up the view plane
	setViewPlane();
}

// display registry

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();



	//call to render fills frameBuffer with pixel colours and draw pixel to screen
	render();

	glutSwapBuffers();

}



// reshape registry

void reshape(int w, int h) {

	glViewport(0, 0, (GLsizei)500, (GLsizei)500);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 200.0);

	glMatrixMode(GL_MODELVIEW);

}

//update the sphere position for the animation
void updateSphere() {
	//increment oribit and keep within 360 degrees
	orbit++;
	orbit %= 360;

	//update animation every 10 degrees
	if (orbit % 10 == 0) {
		float orbitRad = (float)orbit / 180 * PI;

		//calc new x/z positions for spehere
		float newX = orbitMaxX * cos(orbitRad);
		float newZ = orbitMaxZ * sin(orbitRad);
		//create and set new center vector
		vector3 newCenter = vector3(newX + orbitCenterX, orbitCenterY, newZ + orbitCenterZ);
		sphereArray[0].center = newCenter;

		//update display
		glutPostRedisplay();
	}
}

void controls(int button, int state, int x, int y) {

	switch (button) {

		case GLUT_LEFT_BUTTON:
			if (state == GLUT_UP) {
				dStep++;
				dStep %= 10;
				viewer = vector3(130 * cos(dStep * 36 * PI / 180.0), 200.0, 130.0 * sin(dStep * 36 * PI / 180));
				setViewPlane();
			}
			break;

		case GLUT_RIGHT_BUTTON:
			if (state == GLUT_UP) {
				dStep--;
				dStep %= 10;
				viewer = vector3(130 * cos(dStep * 36 * PI / 180.0), 200.0, 130.0 * sin(dStep * 36 * PI / 180));
				setViewPlane();
			}
			break;
	}
	

	
}

// main program

int main(int argc, char **argv)

{

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(500, 500);

	glutInitWindowPosition(100, 100);



	int windowHandle = glutCreateWindow("Ray Trace Test");

	glutSetWindow(windowHandle);

	glutDisplayFunc(display);

	glutReshapeFunc(reshape);

	glutIdleFunc(updateSphere);

	glutMouseFunc(controls);


	initialize();



	glutMainLoop();

	return 0;
}