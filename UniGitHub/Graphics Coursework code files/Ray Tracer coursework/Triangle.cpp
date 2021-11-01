#include "Triangle.h"

Triangle::Triangle(glm::vec3 pointA, glm::vec3 pointB, glm::vec3 pointC) : Shape(pointA, pointB, pointC) {
	colour = glm::vec3(1, 0, 0);
	position = glm::vec3(7.5, 9, -10);
	shininess = 50;
}

Triangle::~Triangle() {

}

void Triangle::Draw() {

}

bool Triangle::CheckRayIntersection(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 direction, glm::vec3 origin, float* t) {
	glm::vec3 e1 = b - a;
	glm::vec3 e2 = c - a;
	glm::vec3 direcE2 = glm::cross(direction, e2);
	glm::vec3 originA = origin - a;
	float denominator = 1 / glm::dot(e1, direcE2);
	float u = glm::dot(originA, direcE2) * denominator;
	float v = glm::dot(direction, glm::cross(originA, e1)) * denominator;

	if (u < 0 || u > 1) {
		return false;
	}
	else if (v < 0 || (u + v) > 1) {
		return false;
	}
	else {
		*t = glm::dot(e2, glm::cross(originA, e1)) * denominator;
		return true;
	}
	return false;
}
