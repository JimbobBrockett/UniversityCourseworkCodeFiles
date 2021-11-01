#include "Landscape.h"

/*
Constructor that passes the position and shader straight to the parent gameobject.
*/
Landscape::Landscape(glm::vec3 pos, Shader& shader) : GameObject(pos, shader) {

}

/*
Basic destructor.
*/
Landscape::~Landscape() {

}

/*
Set the model matrix for the landscape, send it off and call the draw function in the model loader.
*/
void Landscape::Draw() {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.165f, 0.1f, 0.11f));
	int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	modelMesh.Draw(shaderProgram);
}

/*
This is currently unused.
*/
void Landscape::Update() {

}
