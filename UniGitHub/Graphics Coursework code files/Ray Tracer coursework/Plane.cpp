#include "Plane.h"
#include <iostream>

Plane::Plane(glm::vec3 p, glm::vec3 n) : Shape(p, n) {
	colour = glm::vec3(1, 1, 1);
	position = glm::vec3(0, 0, -5);
	shininess = 200;
}

Plane::~Plane() {

}

void Plane::Draw() {
	glPushMatrix();
	glBegin(GL_QUADS);
	glColor3f(colour.x, colour.y, colour.z);
	glVertex3f(-10000, -10000, -5);
	glVertex3f(-10000, 10000, -5);
	glVertex3f(10000, 10000, -5);
	glVertex3f(10000, -10000, -5);
	glEnd();
	glPopMatrix();
}

bool Plane::CheckRayIntersection(glm::vec3 planeNormal, glm::vec3 planePoint, glm::vec3 origin, glm::vec3 direction, float* t) {
	float denominator = glm::dot(direction, planeNormal);
	if (denominator > 1e-6) {
		glm::vec3 pointOriginVector = planePoint - origin;
		float numerator = glm::dot(pointOriginVector, planeNormal);
		float fraction = numerator * (1 / denominator);
		if (fraction > 0) {
			*t = fraction;
			return true;
		}
	}
	return false;
}
