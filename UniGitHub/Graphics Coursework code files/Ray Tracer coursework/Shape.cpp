#include "Shape.h"

Shape::Shape(glm::vec3 pos, glm::vec3 col, float rad) {
	position = pos;
	radius = rad;
	colour = col;
}

Shape::Shape(glm::vec3 p, glm::vec3 n) {
	point = p;
	normal = n;
}

Shape::Shape(glm::vec3 pointA, glm::vec3 pointB, glm::vec3 pointC) {
	A = pointA;
	B = pointB;
	C = pointC;
}

Shape::~Shape() {

}

bool Shape::CheckRayIntersection(glm::vec3 center, glm::vec3 direction, glm::vec3 origin, float radius, float* t) {
	return false;
}

bool Shape::CheckRayIntersection(glm::vec3 planeNormal, glm::vec3 planePoint, glm::vec3 origin, glm::vec3 direction, float* t) {
	return false;
}

bool Shape::CheckRayIntersection(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 direction, glm::vec3 origin, float* t) {
	return false;
}
