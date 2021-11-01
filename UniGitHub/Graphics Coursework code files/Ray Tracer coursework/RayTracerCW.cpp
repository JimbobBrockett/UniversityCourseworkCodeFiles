// RayTracerCW.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <math.h>
#include <cmath>
#include <algorithm>
#include <vector>
#include <glm-0.9.6.3/glm/glm.hpp>
#include "Sphere.h"
#include "Shape.h"
#include "Plane.h"
#include "Triangle.h"

#define PI 3.141592654

std::vector<Shape*> spheres;

/*
	This function calculates the visul colour of the objects.
	It takes in the surface and specular colours, light information, intersect point and object position, ray direction, and shininess.
	It calculates the ambient, diffuse, and specular values for the object, and combines them.
	It returns a single vec3 which determines what the pixel looks like.
*/
glm::vec3 ColourCalculator(glm::vec3 surfaceColour, glm::vec3 specularColour, glm::vec3 light, glm::vec3 lightPos, glm::vec3 intersectPoint, glm::vec3 objectPos, glm::vec3 rayDirection, float shininess) {
	glm::vec3 ambient, diffuse, specular, combined;

	ambient = glm::vec3(surfaceColour.x * light.x, surfaceColour.y * light.y, surfaceColour.z * light.z);

	glm::vec3 l = lightPos - intersectPoint;
	glm::vec3 lNorm = glm::normalize(l);
	glm::vec3 n = intersectPoint - objectPos;
	glm::vec3 nNorm = glm::normalize(n);

	float lDotn = glm::dot(lNorm, nNorm);
	float lnDotMax = glm::max(0.0f, lDotn);

	diffuse = glm::vec3(surfaceColour.x * light.x, surfaceColour.y * light.y, surfaceColour.z * light.z) * lnDotMax;

	glm::vec3 r = glm::normalize(((2 * glm::dot(l, n)) * n) - l);
	float rDotv = glm::dot(r, rayDirection);
	float rvDotMax = glm::max(0.0f, rDotv);
	float maxPower = glm::pow(rvDotMax, shininess);

	specular = glm::vec3(surfaceColour.x * light.x, surfaceColour.y * light.y, surfaceColour.z * light.z) * maxPower;

	combined = ambient + diffuse + specular;

	return combined;
}

/*
	This procedure writes to a .ppm file for opening and viewing the scene.
*/
void WriteToPPMFile(float width, float height, glm::vec3** image) {
	std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			ofs << (unsigned char)(std::min((float)1, (float)image[x][y].x) * 255) <<
				(unsigned char)(std::min((float)1, (float)image[x][y].y) * 255) <<
				(unsigned char)(std::min((float)1, (float)image[x][y].z) * 255);
		}
	}
	ofs.close();
	std::cout << "Written." << std::endl;
}

/*
	The following procedures set up the OpenGL portion ready for real-time implementation.
	This is not being used currently but was placed here in case I made it this far.
*/
void Update(void) {
	glutPostRedisplay();
}

void DrawGame(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 30.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	for (int i = 0; i < 5; i++) {
		spheres[i]->Draw();
	}
	glutSwapBuffers();
}

void Resize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (float)w / (float)h, 1.0, 500.0);
	glMatrixMode(GL_MODELVIEW);
}

void InitEngine(int argc, char** argv, int width, int height) {
	glutInit(&argc, argv);

	glutInitContextVersion(2, 0);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("SphereRayCastTest");
	glutDisplayFunc(DrawGame);
	glutReshapeFunc(Resize);
	glutIdleFunc(Update);

	//glewExperimental = GL_TRUE;
	//glewInit();
}

void Start() {
	glutMainLoop();
}

/*
	This adds object pointers to the vector array.
*/
void AddObjects(Shape* object) {
	spheres.push_back(object);
}

int main(int argc, char** argv)
{
	/*
		These varaibles determine the size of the scene, plane and triangle points and the light information.
	*/
	int width = 640;
	int height = 480;
	glm::vec3 lightColour = glm::vec3(0.5, 0.5, 0.5);
	glm::vec3 lightPosition = glm::vec3(-20, 20, 0);
	glm::vec3 specularColour = glm::vec3(0.7, 0.7, 0.7);
	glm::vec3 planeN = glm::normalize(glm::vec3(0, -1, 0));
	glm::vec3 planeP = glm::vec3(0, -10, -50);
	glm::vec3 triangleA = glm::vec3(5, 5, -10);
	glm::vec3 triangleB = glm::vec3(5, 13, -10);
	glm::vec3 triangleC = glm::vec3(10, 6, -10);

	/*
		The instances of the objects are added here.
	*/
	AddObjects(new Triangle(triangleA, triangleB, triangleC));
	AddObjects(new Plane(planeP, planeN));
	AddObjects(new Sphere(glm::vec3(0, 0, -20), glm::vec3(1.0, 0.32, 0.36), 4)); //red
	AddObjects(new Sphere(glm::vec3(5, -1, -15), glm::vec3(0.9, 0.76, 0.46), 2)); //yellow
	AddObjects(new Sphere(glm::vec3(5, 0, -25), glm::vec3(0.65, 0.77, 0.97), 3)); //blue
	AddObjects(new Sphere(glm::vec3(-5.5, 0, -15), glm::vec3(0.9, 0.9, 0.9), 3)); //grey

	InitEngine(argc, argv, width, height);
	//Start();

	//sphere1.Draw();

	/*
		These variables determine the camera and intersection information.
	*/
	float ImageAspectRatio, tanVal, PixelNormX, PixelNormY, PixelRemapX, PixelRemapY, PixelCamX, PixelCamY;
	float tParameter = 0;
	bool intersection;
	glm::vec3 CameraOrigin = glm::vec3(0, 0, 0);
	glm::vec3 rayDirection;

	/*
		This creates the 2D array of pointers to vec3's.
	*/
	glm::vec3** image = new glm::vec3 * [width];
	for (int i = 0; i < width; i++) {
		image[i] = new glm::vec3[height];
	}

	ImageAspectRatio = (float)width / (float)height;
	tanVal = (float)tanf((float)60.0 * (float)(PI / (float)180.0));
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; ++x) {
			/*
				The first section of this for loop sets up where the pixel is compared to the camera, and where the ray cast points.
			*/
			image[x][y] = glm::vec3(0, 0, 0);
			tParameter = 10000;
			PixelNormX = (x + 0.5f) / width;
			PixelNormY = (y + 0.5f) / height;

			PixelRemapX = ((2 * PixelNormX) - 1) * ImageAspectRatio;
			PixelRemapY = 1 - (2 * PixelNormY);

			PixelCamX = PixelRemapX * tanVal;
			PixelCamY = PixelRemapY * tanVal;

			rayDirection.x = PixelCamX - CameraOrigin.x;
			rayDirection.y = PixelCamY - CameraOrigin.y;
			rayDirection.z = -1.0f - CameraOrigin.z;
			rayDirection = glm::normalize(rayDirection);

			/*
				This section checks the intersection for each object, and then calculates the colour and lighting effects.
			*/
			intersection = spheres[1]->CheckRayIntersection(planeN, planeP, CameraOrigin, rayDirection, &tParameter);
			if (intersection) {
				image[x][y] = ColourCalculator(spheres[1]->colour, specularColour, lightColour, lightPosition, rayDirection * tParameter, planeP, rayDirection, spheres[1]->shininess);
			}
			intersection = spheres[0]->CheckRayIntersection(triangleA, triangleB, triangleC, rayDirection, CameraOrigin, &tParameter);
			if (intersection) {
				image[x][y] = ColourCalculator(spheres[0]->colour, specularColour, lightColour, lightPosition, rayDirection * tParameter, spheres[0]->position, rayDirection, spheres[0]->shininess);
			}
			for (int i = 2; i < 6; i++) {
				intersection = spheres[i]->CheckRayIntersection(spheres[i]->position, rayDirection, CameraOrigin, spheres[i]->radius, &tParameter);
				if (intersection) {
					image[x][y] = ColourCalculator(spheres[i]->colour, specularColour, lightColour, lightPosition, rayDirection * tParameter, spheres[i]->position, rayDirection, spheres[i]->shininess);
				}
			}
		}
	}

	WriteToPPMFile((float)width, (float)height, image);

	//Start();
}