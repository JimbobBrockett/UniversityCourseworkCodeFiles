#include "GameObject.h"

/*
This constructs the main GameObject class. A position and shader are passed in.
*/
GameObject::GameObject(glm::vec3 pos, Shader& shader) : shaderProgram(shader) {
	position = pos;
}

/*
Basic deconstructor.
*/
GameObject::~GameObject() {

}

/*
Update loop that will be filled in child classes.
*/
void GameObject::Update() {

}

/*
This returns the position of the object.
*/
glm::vec3 GameObject::GetPosition() {
	return position;
}

/*
This receives a vec3 that gets added to the current position.
*/
void GameObject::SetPosition(glm::vec3 movement) {
	position += movement;
}

/*
This sets the rotation of Zidane only.
*/
void GameObject::SetRotation(float angle) {
	ZidaneAngle = angle; 
}