#include "Sphere.h"

Sphere::Sphere(glm::vec3 pos, glm::vec3 col, float rad) : Shape(pos, col, rad) {
	shininess = 50;
}

Sphere::~Sphere() {

}

void Sphere::Draw() {
	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);
	glColor3f(colour.x, colour.y, colour.z);
	glutSolidSphere(radius, 30, 30);
	glPopMatrix();
}

bool Sphere::CheckRayIntersection(glm::vec3 center, glm::vec3 direction, glm::vec3 origin, float radius, float* t) {
	float t0, t1, tca;

	glm::vec3 L = center - origin;
	tca = glm::dot(L, direction);
	if (tca < 0) {
		return false;
	}

	float s = glm::dot(L, L) - (tca * tca);
	if (s > (radius * radius)) {
		return false;
	}

	float thc = sqrt((radius * radius) - s);
	t0 = tca - thc;
	t1 = tca + thc;
	if (t0 > t1) {
		std::swap(t0, t1);
	}

	if (t0 < 0) {
		t0 = t1;
		if (t0 < 0) {
			return false;
		}
	}

	if (t0 < *t) {
		*t = t0;
		return true;
	}

	return false;
}