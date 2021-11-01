// CarParkPuzzle.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "GameEngine.h"

int main()
{
	GameEngine::InitiateOpenGL(); // Call the start function.
}

/*
The int main() function in here is used purely to start the engine. If I did not call
	it here, the engine would never start as the code would go through int main(), not 
	see anything and then think that the program had ended there. I do believe there
	are ways around this where int main() is not included at all, however I do not wish
	to experiment with this.

I have attempted all subsystems in some way. Together, they create an engine that I have
	used to make a game. The game is in the style of the old car park games, where the player
	must manoeuvre the normal blue cars to free the target green car. The gameplay is simplistic,
	but highly suitable and functions excellently.

OPENGL 
For the graphics system, I have used GLFW and GLAD. Together, these give me access to all of
	the OpenGL functions I need to be able to complete the graphics side of the code. There are a 
	range of different options out there for both libraries, but I chose these two as there is a 
	very handy online tutorial that I could refer to if I got stuck at any point called LearnOpenGl.
	Using these I have successfully created shaders and linked them to a shader program, binded vertex
	arrays, vertex buffers and element buffers, open a window and render all of these to the window.

SHADERS
The shaders I have used are suited for rendering materials over textures, as I have not used texture 
	coordinates in them. The vertex shader holds uniforms for the model, view and projection matrices
	for the viewport, and creates a fragment position and gl_position from these. The normal of the given
	model matrix is also calculated, and both this and the fragment position are output to the fragment
	shader. The fragment shader holds structs for a material and a light, which in this case is a direction
	light instead of a point light. I chose this kind of light as I wanted the entire scene to be lit equally. 
	Uniforms for a view positon (camera position), material data, and light data are ready to have data passed
	to them, and the fragment position and normal are also passed in from the vertex shader. Lighting calculations
	are completed for ambient, diffuse, and specular lighting, the three are added together in a vec3, and this
	is used to create a fragment colour which is output to the screen.

ASSIMP
The objects are being loaded using Assimp. This loads a range of different object types, including
	files straight from Blender, but I am using this to load .obj files. This grabs the vertex data
	using nodes, and also loads texture or material data. As my models don't have textures applied,
	I am using Assimp to load the materials instead.

BOX2D
The physics library I am using is Box2D. The reason I chose this is because even though the program
	loads 3D objects, all of those objects only ever move in 2 dimensions (technically one dimension
	per car) meaning that a 3D physics library was not necessary. Along with this, I have also used
	purely C++ to implement a grid system. This keeps track of where each car is inside the car park,
	and updates every time a car is moved.

IMGUI
The UI has been completed using the ImGui library. This renders windows on top of the shown screen, which can 
	hold any data I choose. The main windows related to the gameplay are a move counter window and a winning 
	window that only appears when the level has been completed. The user can interact with these windows, ticking
	check boxes that will decide whether or not they display debug data or not. 

FMOD
The audio library I chose to use was FMOD. I know WWise was also an option, but I heard about issues other people
	had when attempting to use this library (files missing from download), so I opted for FMOD instead. After 
	downloading this, I also downloaded some free sounds that I can play. The sounds themselves are not of the 
	best quality, but they play correctly when run through FMOD which is the main thing. 

GAMEPLAY
The main goal of the game I have created is to manoeuvre the blue cars in such a way that the green car can reach 
	the exit clearly shown on the right of the screen. The player first clicks the car they wish to move, which 
	activates that car and deactivates any previously active car. This then allows them to use WASD controls to 
	move the car. If the move is legal, the car moves, the grid is updated, and the new position data is put into
	a data packet that is sent over to the server via the network connection. Once the target green car has reached
	its goal position, a UI window appears that deactivates most of the interaction procedures and tells the user
	that they have successfully won the game. If the escape button is pressed at any point during the game, the 
	window closes and the game terminates.

NETWORKING
I have used ENet for the networking. This is the only subsystem that needs other instances of VS open. There is a 
	separate .sln file that acts as a server, and then I can open multiple instances of this file and run them as 
	clients. While connected, when a legal move is made, this new position is sent to the server and then the server
	broadcasts this to the rest of the clients. When a client presses the escape button and shuts down the window, 
	this disconnects them from the server as well. The server keeps track of how many clients are connected, and 
	after a client disconnects and the number of connected clients goes down, it checks to see if this has gone to 
	0. If the number of clients has decreased to 0, then all clients have disconnected and the game has ended, so
	the server shuts down and that program stops running.

DATA INPUT
Separate to the main files, there is a .txt file that contains the following data:
	MA, -1.1, 1.1, 0.0
	NC, -0.75, 0.375, 0.0|V
	NC, -0.75, -0.375, 0.0|V
	NC, -0.375, 0.5, 0.0|H
	NC, -0.375, -0.25, 0.0|H
	NC, -0.125, 0.25, 0.0|H
	NC, 0.0, -0.125, 0.0|V
	NC, -0.125, -0.5, 0.0|H
	NC, 0.375, -0.5, 0.0|H
	NC, 0.75, -0.375, 0.0|V
	NC, 0.5, -0.125, 0.0|V
	NC, 0.75, 0.125, 0.0|V
	NC, 0.625, 0.5, 0.0|H
	TC, -0.5, 0.0, 0.0
	LS, 0.0, 0.0, -1.0
	The code reads this text file and runs a small piece of code to parse it. The first two characters are taken as
	the object type, which are Modelling Asset, Normal Car, Target Car, and LandScape. The three numbers that follow 
	are the x, y, z coordinates of the object. For the normal cars, this is followed by a | and then a single 
	character, either H or V. This represents either Horizontal or Vertical, and determines the orientation of the 
	car. 

DATA OUTPUT
This has already been mentioned in the NETWORKING comments. I transmit position data across to other clients via a 
	server, which receives data packets containing the new position data and then broadcasts this to all other 
	clients.
*/