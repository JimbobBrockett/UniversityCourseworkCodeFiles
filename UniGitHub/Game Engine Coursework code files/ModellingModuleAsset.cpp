#include "ModellingModuleAsset.h"

/*
This is the constructor. The position and shader are passed in and used in the parent
	GameObject class.
*/
ModellingModuleAsset::ModellingModuleAsset(glm::vec3 pos, Shader& shader) : GameObject(pos, shader) {
	
}

/*
Basic deconstructor.
*/
ModellingModuleAsset::~ModellingModuleAsset() {

}

/*
This sets the model matrix for the asset and then draws it to the screen.
*/
void ModellingModuleAsset::Draw() {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(ZidaneAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
	int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	modelMesh.Draw(shaderProgram);
}

/*
The update loop for the object. As it is a static object, there won't be anything in here.
*/
void ModellingModuleAsset::Update() {

}