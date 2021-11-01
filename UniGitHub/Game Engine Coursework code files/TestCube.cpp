#include "TestCube.h"

TestCube::TestCube(glm::vec3 pos, glm::vec3 col) {
	position = pos;
	colour = col;
}

TestCube::~TestCube() {

}

void TestCube::Draw(unsigned int* vbo, unsigned int* ebo) {
	// Bind the buffer array.
	glBindBuffer(GL_ARRAY_BUFFER, (*vbo));
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Bind the element array.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*ebo));
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Set the attribute pointers.
	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// The vbo is already the vertex attributes bound buffer, so unbinding is safe.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}