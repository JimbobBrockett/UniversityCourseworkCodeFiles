#include "TargetCar.h"

/*
Constructor that passes the position and shader straight to the parent gameobject.
*/
TargetCar::TargetCar(glm::vec3 pos, Shader& shader) : GameObject(pos, shader) {

}

/*
Basic destructor.
*/
TargetCar::~TargetCar() {

}

/*
Set the model matrix for the car, send it to the shader and then call the draw function
	in the model loader. If this is the active car, add further transformations to the 
	model matrix and draw a cube above the car that shows which car is active.
*/
void TargetCar::Draw() {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.09f));
	int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	modelMesh.Draw(shaderProgram);
	if (active) {
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ActiveCube.Draw(shaderProgram);
	}
}

void TargetCar::Update() {
	
}
