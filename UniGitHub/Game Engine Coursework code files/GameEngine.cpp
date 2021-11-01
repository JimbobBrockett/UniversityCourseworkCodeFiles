#include "GameEngine.h"

std::vector<GameObject*> GameEngine::objects;
std::vector<b2Body*> GameEngine::bodies;
bool GameEngine::pressed;
bool GameEngine::keyPressed;
bool GameEngine::showMoveCounter = true;
bool GameEngine::playHorn = false;
bool GameEngine::displayDebug = true;
bool GameEngine::win = false;
int GameEngine::movesTaken = 0;
int GameEngine::ZidaneMovePhase = 1;
float GameEngine::movementIncrement = 0.25f;
float GameEngine::ZidaneAngle = 45.0f;
int GameEngine::grid[7][5];

/*
These functions aid GLFW in processing several things, such as resizing the window and processing user input.
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

/*
This initiates the graphics and begins the render loop. This will be one of the last things initiated as once
	I enter that loop, I cannot leave until I close the window. 
*/
void GameEngine::InitiateOpenGL() {
	// Create a window, and set the callback functions for resizing the window and cursor callback.
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 800, "CarParkPuzzle", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window." << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Set the input mode for the cursor.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// Load all OpenGL function pointers.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialise GLAD." << std::endl;
	}

	// Enable the Z-Depth buffer.
	glEnable(GL_DEPTH_TEST);

	// Create a shader program.
	Shader shaderProgram("VertexShader.txt", "FragmentShader.txt");	

	// Create the physics world to attach bodies onto.
	// I also create the body for Zidane here.
	b2Vec2 gravity(0.0f, -9.81f);
	b2World world(gravity);
	b2PolygonShape ZidaneBox;
	ZidaneBox.SetAsBox(0.5f, 0.25f);
	b2BodyDef ZidaneBodyDef;
	ZidaneBodyDef.type = b2_dynamicBody;
	ZidaneBodyDef.position.Set(-1.0f, 1.1f);
	ZidaneBodyDef.angle = glm::radians(ZidaneAngle);
	ZidaneBodyDef.fixedRotation = true;
	b2Body* Zidane = world.CreateBody(&ZidaneBodyDef);
	b2FixtureDef ZidaneFixtureDef;
	ZidaneFixtureDef.shape = &ZidaneBox;
	ZidaneFixtureDef.density = 0.3f;
	ZidaneFixtureDef.friction = 0.3f;
	Zidane->CreateFixture(&ZidaneFixtureDef);

	// Create the objects that will be in the game.
	CreateObjects(shaderProgram, world);

	// Set up the grid that keeps track of where the cars are currently positioned.
	// This grid will change every time a legal move is made.
	InitialiseGrid();

	// Set up the camera position and rotation.
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	// Set up a light.
	glm::vec3 lightDir = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 lightAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	glm::vec3 lightDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 lightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);

	// Set the mouse button pressed bool to false.
	pressed = false;

	// Initiate the UI.
	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();

	// Initiate the sounds. Start the looping engine sound.
	FMOD::Studio::System* system = NULL;
	FMOD::Studio::System::create(&system);
	FMOD::System* lowLevelSystem = NULL;
	system->getLowLevelSystem(&lowLevelSystem);
	lowLevelSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_STEREO, 0);
	system->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, NULL);
	FMOD::Sound* engine = NULL;
	lowLevelSystem->createSound("Sounds/Engine.mp3", FMOD_LOOP_NORMAL, NULL, &engine);
	FMOD::Sound* horn = NULL;
	lowLevelSystem->createSound("Sounds/Horn.wav", FMOD_LOOP_OFF, NULL, &horn);
	engine->setDefaults(16400, 0);
	horn->setDefaults(16400, 0);
	FMOD::Channel* engineChannel = NULL;
	FMOD::Channel* hornChannel = NULL;
	lowLevelSystem->playSound(engine, NULL, false, &engineChannel);

	// Set up this VS instance as a client.
	if (enet_initialize() != 0) {
		std::cout << "Enet initialisation failed." << std::endl;
	}
	ENetAddress address;
	ENetHost* client;
	ENetPeer* peer;
	ENetEvent enetEvent;
	client = enet_host_create(NULL, 1, 2, 0, 0);
	if (client == NULL) {
		std::cout << "Client initialisation failed." << std::endl;
	}
	enet_address_set_host(&address, "localhost");
	address.port = 140110;
	peer = enet_host_connect(client, &address, 2, 0);
	if (peer == NULL) {
		std::cout << "No available peers for initializing an ENet connection." << std::endl;
	}
	ClientData* clientData = new ClientData;
	int clientIndex = -1;
	PhysicsData* receivedData = new PhysicsData;

	// The game loop.
	GameLoop(window, shaderProgram, cameraPos, cameraFront, up, lightDir, lightDiffuse, lightAmbient, lightSpecular, world, Zidane, lowLevelSystem, horn, hornChannel, peer, client, enetEvent, clientData, clientIndex, receivedData);

	// Shutdown and destory the UI elements.
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

	// Deinitialise Enet.
	enet_host_destroy(client);
	enet_deinitialize();

	// Terminate GLFW program.
	glfwDestroyWindow(window);
	glfwTerminate();
}

/*
This is a callback function for GLFW to use when resizing the window.
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

/*
This contains all the input data for the program, including any keys the user may press and any mouse inputs.
*/
void GameEngine::processInput(GLFWwindow* window, ENetPeer* peer, int& clientIndex) {
	// End the game.
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		if (peer != NULL) {
			enet_peer_disconnect_now(peer, 0);
		}
		glfwSetWindowShouldClose(window, true);
	}

	if (!win) {
		// Set the mouse button input and mouse position variables
		int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		double mouseX, mouseY;

		// If the left mouse button is clicked, set the active car once
		if (state == GLFW_PRESS && !pressed) {
			// Set all actives to be false.
			for (int i = 0; i < objects.size(); i++) {
				objects[i]->active = false;
			}

			// Get mouse position and alter to fit world space
			glfwGetCursorPos(window, &mouseX, &mouseY);
			mouseX = ((mouseX / 400) - 1) * (4.0f / 3.0f);
			mouseY = (((mouseY / 400) - 1) * -1) * (4.0f / 3.0f); // These two lines set the values to the same range as the viewable object range.

			// Set the clicked car to active
			for (int i = 0; i < objects.size(); i++) {
				if (dynamic_cast<NormalCar*>(objects[i]) != nullptr) {
					NormalCar* temp = dynamic_cast<NormalCar*>(objects[i]);
					if (temp->HoriVert == 'H') {
						if (mouseX < temp->GetPosition().x + 0.25f && mouseX > temp->GetPosition().x - 0.25f) {
							if (mouseY < temp->GetPosition().y + 0.125f && mouseY > temp->GetPosition().y - 0.125f) {
								temp->active = true;
							}
						}
					}
					else if (temp->HoriVert == 'V') {
						if (mouseX < temp->GetPosition().x + 0.125f && mouseX > temp->GetPosition().x - 0.125f) {
							if (mouseY < temp->GetPosition().y + 0.25f && mouseY > temp->GetPosition().y - 0.25f) {
								temp->active = true;
							}
						}
					}
				}
				else if (dynamic_cast<TargetCar*>(objects[i]) != nullptr) {
					TargetCar* temp = dynamic_cast<TargetCar*>(objects[i]);
					if (mouseX < temp->GetPosition().x + 0.375f && mouseX > temp->GetPosition().x - 0.375f) {
						if (mouseY < temp->GetPosition().y + 0.125f && mouseY > temp->GetPosition().y - 0.125f) {
							temp->active = true;
						}
					}
				}
			}

			// Set boolean so this cannot run multiple times while left mouse button is clicked
			pressed = true;
		}

		// When the left mouse button is released, reset the boolean so that another car can be active.
		if (!state == GLFW_PRESS) {
			pressed = false;
		}

		// Move the currently active car if it points the right way
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && !keyPressed) {
			for (int i = 0; i < objects.size(); i++) {
				if (objects[i]->active) {
					if (dynamic_cast<NormalCar*>(objects[i]) != nullptr) {
						NormalCar* temp = dynamic_cast<NormalCar*>(objects[i]);
						if (temp->HoriVert == 'V') {
							int tempX = (temp->GetPosition().x + 0.75) * 4;
							int tempY = 4 - ((temp->GetPosition().y + 0.375) * 4);
							if (tempY - 1 != 0) {
								if (grid[tempX][tempY - 2] == 0) {
									temp->SetPosition(glm::vec3(0.0f, movementIncrement, 0.0f));
									InitialiseGrid();
									movesTaken++;

									// Send the updated position to the server.
									ENetPacket* dataPacket;
									ClientPacket* data = new ClientPacket;
									data->clientIndex = clientIndex;
									for (int i = 0; i < objects.size(); i++) {
										data->position.x = objects[i]->GetPosition().x;
										data->position.y = objects[i]->GetPosition().y;
										data->position.z = objects[i]->GetPosition().z;
										data->objectIndex = i;
										dataPacket = enet_packet_create(data, sizeof(ClientPacket), ENET_PACKET_FLAG_RELIABLE);
										enet_peer_send(peer, 0, dataPacket);
									}
								}
								// Play the horn sound if an incorrcect move is made.
								else {
									playHorn = true;
								}
							}
							else {
								playHorn = true;
							}
						}
					}
				}
			}
			keyPressed = true;
		}
		else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && !keyPressed) {
			for (int i = 0; i < objects.size(); i++) {
				if (objects[i]->active) {
					if (dynamic_cast<NormalCar*>(objects[i]) != nullptr) {
						NormalCar* temp = dynamic_cast<NormalCar*>(objects[i]);
						if (temp->HoriVert == 'H') {
							int tempX = (temp->GetPosition().x + 0.625) * 4;
							int tempY = 4 - ((temp->GetPosition().y + 0.5) * 4);
							if (tempX != 0) {
								if (grid[tempX - 1][tempY] == 0) {
									temp->SetPosition(glm::vec3(-movementIncrement, 0.0f, 0.0f));
									InitialiseGrid();
									movesTaken++;
									ENetPacket* dataPacket;
									ClientPacket* data = new ClientPacket;
									data->clientIndex = clientIndex;
									for (int i = 0; i < objects.size(); i++) {
										data->position.x = objects[i]->GetPosition().x;
										data->position.y = objects[i]->GetPosition().y;
										data->position.z = objects[i]->GetPosition().z;
										data->objectIndex = i;
										dataPacket = enet_packet_create(data, sizeof(ClientPacket), ENET_PACKET_FLAG_RELIABLE);
										enet_peer_send(peer, 0, dataPacket);
									}
								}
								else {
									playHorn = true;
								}
							}
							else {
								playHorn = true;
							}
						}
					}
					else if (dynamic_cast<TargetCar*>(objects[i]) != nullptr) {
						int tempX = (objects[i]->GetPosition().x + 0.5) * 4;
						int tempY = 4 - ((objects[i]->GetPosition().y + 0.5) * 4);
						if (tempX != 0) {
							if (grid[tempX - 1][tempY] == 0) {
								objects[i]->SetPosition(glm::vec3(-movementIncrement, 0.0f, 0.0f));
								InitialiseGrid();
								movesTaken++;
								ENetPacket* dataPacket;
								ClientPacket* data = new ClientPacket;
								data->clientIndex = clientIndex;
								for (int i = 0; i < objects.size(); i++) {
									data->position.x = objects[i]->GetPosition().x;
									data->position.y = objects[i]->GetPosition().y;
									data->position.z = objects[i]->GetPosition().z;
									data->objectIndex = i;
									dataPacket = enet_packet_create(data, sizeof(ClientPacket), ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, dataPacket);
								}
							}
							else {
								playHorn = true;
							}
						}
						else {
							playHorn = true;
						}
					}
				}
			}
			keyPressed = true;
		}
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && !keyPressed) {
			for (int i = 0; i < objects.size(); i++) {
				if (objects[i]->active) {
					if (dynamic_cast<NormalCar*>(objects[i]) != nullptr) {
						NormalCar* temp = dynamic_cast<NormalCar*>(objects[i]);
						if (temp->HoriVert == 'V') {
							int tempX = (temp->GetPosition().x + 0.75) * 4;
							int tempY = 4 - ((temp->GetPosition().y + 0.375) * 4);
							if (tempY != 4) {
								if (grid[tempX][tempY + 1] == 0) {
									temp->SetPosition(glm::vec3(0.0f, -movementIncrement, 0.0f));
									InitialiseGrid();
									movesTaken++;
									ENetPacket* dataPacket;
									ClientPacket* data = new ClientPacket;
									data->clientIndex = clientIndex;
									for (int i = 0; i < objects.size(); i++) {
										data->position.x = objects[i]->GetPosition().x;
										data->position.y = objects[i]->GetPosition().y;
										data->position.z = objects[i]->GetPosition().z;
										data->objectIndex = i;
										dataPacket = enet_packet_create(data, sizeof(ClientPacket), ENET_PACKET_FLAG_RELIABLE);
										enet_peer_send(peer, 0, dataPacket);
									}
								}
								else {
									playHorn = true;
								}
							}
							else {
								playHorn = true;
							}
						}
					}
				}
			}
			keyPressed = true;
		}
		else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && !keyPressed) {
			for (int i = 0; i < objects.size(); i++) {
				if (objects[i]->active) {
					if (dynamic_cast<NormalCar*>(objects[i]) != nullptr) {
						NormalCar* temp = dynamic_cast<NormalCar*>(objects[i]);
						if (temp->HoriVert == 'H') {
							int tempX = (temp->GetPosition().x + 0.625) * 4;
							int tempY = 4 - ((temp->GetPosition().y + 0.5) * 4);
							if (tempX + 1 != 6) {
								if (grid[tempX + 2][tempY] == 0) {
									temp->SetPosition(glm::vec3(movementIncrement, 0.0f, 0.0f));
									InitialiseGrid();
									movesTaken++;
									ENetPacket* dataPacket;
									ClientPacket* data = new ClientPacket;
									data->clientIndex = clientIndex;
									for (int i = 0; i < objects.size(); i++) {
										data->position.x = objects[i]->GetPosition().x;
										data->position.y = objects[i]->GetPosition().y;
										data->position.z = objects[i]->GetPosition().z;
										data->objectIndex = i;
										dataPacket = enet_packet_create(data, sizeof(ClientPacket), ENET_PACKET_FLAG_RELIABLE);
										enet_peer_send(peer, 0, dataPacket);
									}
								}
								else {
									playHorn = true;
								}
							}
							else {
								playHorn = true;
							}
						}
					}
					else if (dynamic_cast<TargetCar*>(objects[i]) != nullptr) {
						int tempX = (objects[i]->GetPosition().x + 0.5) * 4;
						int tempY = 4 - ((objects[i]->GetPosition().y + 0.5) * 4);
						if (tempX + 2 != 6) {
							if (grid[tempX + 3][tempY] == 0) {
								objects[i]->SetPosition(glm::vec3(movementIncrement, 0.0f, 0.0f));
								InitialiseGrid();
								movesTaken++;
								ENetPacket* dataPacket;
								ClientPacket* data = new ClientPacket;
								data->clientIndex = clientIndex;
								for (int i = 0; i < objects.size(); i++) {
									data->position.x = objects[i]->GetPosition().x;
									data->position.y = objects[i]->GetPosition().y;
									data->position.z = objects[i]->GetPosition().z;
									data->objectIndex = i;
									dataPacket = enet_packet_create(data, sizeof(ClientPacket), ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, dataPacket);
								}
							}
							else {
								playHorn = true;
							}
						}
						else {
							playHorn = true;
						}
						// If the target car is in the right position, player wins.
						if (tempX + 4 == 7) {
							displayDebug = false;
							win = true;
						}
					}
				}
			}
			keyPressed = true;
		}
		if (glfwGetKey(window, GLFW_KEY_W) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_A) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) != GLFW_PRESS) {
			keyPressed = false;
		}
	}
	// If the target car is in the right position, player wins.
	if (((objects[13]->GetPosition().x + 0.5) * 4) + 4 == 8) {
		displayDebug = false;
		win = true;
	}
}

/*
This function is for adding the objects to an array.
*/
void GameEngine::AddObject(GameObject* object) {
	objects.push_back(object);
}

/*
This function is for creating all the objects. It parses a text file containing the 
	initial positions for the assets, and then creates the objects using these positions.
*/
void GameEngine::CreateObjects(Shader& shaderProgram, b2World& world) {
	glm::vec3 tempPosition;
	std::ifstream positionFile;
	std::string line, objectType, xS, yS, zS;
	float x, y, z;
	int coordinate = 1;
	char orientation;
	// Open the text file.
	positionFile.open("PositionData.txt");
	while (!positionFile.eof()) {
		std::getline(positionFile, line);
		// Reset all the values.
		coordinate = 1;
		xS = "";
		yS = "";
		zS = "";
		objectType = "";
		// First two values are the object type.
		objectType = line[0];
		objectType += line[1];
		for (int i = 4; i < line.size(); i++) {
			if (line[i] != ',' && line[i] != ' ' && line[i] != '|' && line[i] != 'V' && line[i] != 'H') {
				// These are the string versions of the coordinates in each line.
				if (coordinate == 1) {
					xS += line[i];
				}
				else if (coordinate == 2) {
					yS += line[i];
				}
				else if (coordinate == 3) {
					zS += line[i];
				}
			}
			else if (line[i] == ' ') {
				// A space means the next coordinate component is coming up.
				coordinate++;
			}
			else if (line[i] == '|') {
				// The normal cars have an orientation, which is shown after this symbol.
				orientation = line[i + 1];
			}
		}
		// Convert the strings to floats.
		x = std::stof(xS);
		y = std::stof(yS);
		z = std::stof(zS);
		// Set the position components of the temporary vec3.
		tempPosition.x = x;
		tempPosition.y = y;
		tempPosition.z = z;
		if (objectType == "MA") {
			// 302CDE asset
			AddObject(new ModellingModuleAsset(tempPosition, shaderProgram));
		}
		else if (objectType == "NC") {
			// Normal cars
			AddObject(new NormalCar(tempPosition, shaderProgram, orientation, world));
		}
		else if (objectType == "TC") {
			// The target car
			AddObject(new TargetCar(tempPosition, shaderProgram));
		}
		else if (objectType == "LS") {
			// The car park that the cars drive around
			AddObject(new Landscape(tempPosition, shaderProgram));
		}
	}

	// Close the file.
	positionFile.close();

	// Check all objects have loaded.
	if (objects.size() == 15) {
		std::cout << "All objects loaded successfully." << std::endl;
	}
}

/*
This is the render loop. The game takes place in here. The following functions are done in this order:
	Process user input
	Draw and render the objects
	Update the networking
	Update the physics of the objects
	Update the UI
	Run the audio
*/
void GameEngine::GameLoop(GLFWwindow* window, Shader& shaderProgram, glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 up, glm::vec3 lightDirection, glm::vec3 lightDiffuse, glm::vec3 lightAmbient, glm::vec3 lightSpecular, b2World& world, b2Body* Zidane, FMOD::System* lowLevelSystem, FMOD::Sound* horn, FMOD::Channel* hornChannel, ENetPeer* peer, ENetHost* client, ENetEvent& enetEvent, ClientData* clientData, int& clientIndex, PhysicsData* receivedData) {
	while (!glfwWindowShouldClose(window)) {
		// Draw and render the scene.
		Render(window, shaderProgram, cameraPos, cameraFront, up, lightDirection, lightDiffuse, lightAmbient, lightSpecular);
		
		// Process user input.
		processInput(window, peer, clientIndex);

		// Update the networking.
		Network(client, enetEvent, clientData, clientIndex, peer, receivedData);

		// Update the physics.
		Physics(world, Zidane);

		// Update the UI in the scene. This has been moved to inside the Render() procedure.
		//UI();

		// Update the audio.
		Audio(lowLevelSystem, horn, hornChannel);
	}
}

/*
This is the render function that sets up the light, camera and draws the objects onto the scene.
*/
void GameEngine::Render(GLFWwindow* window, Shader& shaderProgram, glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 up, glm::vec3 lightDirection, glm::vec3 lightDiffuse, glm::vec3 lightAmbient, glm::vec3 lightSpecular) {
	// Set the background colour, and clear colour and depth bits.
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Activate the shader program, set the colours, and set the light.
	shaderProgram.Use();

	// Use the light.
	shaderProgram.SetVec3("light.direction", lightDirection.x, lightDirection.y, lightDirection.z);
	shaderProgram.SetVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
	shaderProgram.SetVec3("light.ambient", lightAmbient.x, lightAmbient.y, lightAmbient.z);
	shaderProgram.SetVec3("light.diffuse", lightDiffuse.x, lightDiffuse.y, lightDiffuse.z);
	shaderProgram.SetVec3("light.specular", lightSpecular.x, lightSpecular.y, lightSpecular.z);

	// Set up view and projection matrices.
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection;
	view = glm::lookAt(cameraPos, cameraFront, up);
	projection = glm::perspective(glm::radians(45.0f), 800.0f / 800.0f, 0.1f, 100.0f);

	// Send the view and projection matrices to the shader and use them.
	int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
	int projectionLoc = glGetUniformLocation(shaderProgram.ID, "projection");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// Draw the model.
	for (int i = 0; i < objects.size(); i++) {
		objects[i]->Draw();
	}

	// Call the function that creates the UI boxes and renders them to the screen.
	UI();

	// Swap the buffers and check for other events.
	glfwSwapBuffers(window);
	glfwPollEvents();
}

/*
This is the physics function that solves the collision detection for the cars.
*/
void GameEngine::Physics(b2World& world, b2Body* Zidane) {
	// Increase the angle that Zidane points at and get his position.
	ZidaneAngle++;
	glm::vec3 ZidanePosition = objects[0]->GetPosition();

	// Box2D step using the framerate grabbed from ImGui.
	world.Step(1.0f / ImGui::GetIO().Framerate, 8, 3);
	
	// Move Zidane in a square around the car park.
	if (ZidaneMovePhase == 1) {
		Zidane->SetTransform(b2Vec2(Zidane->GetPosition().x + 0.001f, Zidane->GetPosition().y), glm::radians(ZidaneAngle));
		objects[0]->SetPosition(glm::vec3(Zidane->GetPosition().x - ZidanePosition.x, 0.0f, 0.0f));
		objects[0]->SetRotation(ZidaneAngle);
	}
	else if (ZidaneMovePhase == 2) {
		Zidane->SetTransform(b2Vec2(Zidane->GetPosition().x, Zidane->GetPosition().y - 0.001f), glm::radians(ZidaneAngle));
		objects[0]->SetPosition(glm::vec3(0.0f, -0.001f, 0.0f));
		objects[0]->SetRotation(ZidaneAngle);
	}
	else if (ZidaneMovePhase == 3) {
		Zidane->SetTransform(b2Vec2(Zidane->GetPosition().x - 0.001f, Zidane->GetPosition().y), glm::radians(ZidaneAngle));
		objects[0]->SetPosition(glm::vec3(Zidane->GetPosition().x - ZidanePosition.x, 0.0f, 0.0f));
		objects[0]->SetRotation(ZidaneAngle);
	}
	else if (ZidaneMovePhase == 4) {
		Zidane->SetTransform(b2Vec2(Zidane->GetPosition().x, Zidane->GetPosition().y + 0.001f), glm::radians(ZidaneAngle));
		objects[0]->SetPosition(glm::vec3(0.0f, 0.001f, 0.0f));
		objects[0]->SetRotation(ZidaneAngle);
	}

	// Once Zidane has reach corners of the car park, change the direction he moves in.
	if (objects[0]->GetPosition().x <= -1.1f && objects[0]->GetPosition().y >= 1.1f) {
		ZidaneMovePhase = 1;
	}
	else if (objects[0]->GetPosition().x >= 1.1f && objects[0]->GetPosition().y >= 1.1f) {
		ZidaneMovePhase = 2;
	}
	else if (objects[0]->GetPosition().x >= 1.1f && objects[0]->GetPosition().y <= -1.1f) {
		ZidaneMovePhase = 3;
	}
	else if (objects[0]->GetPosition().x <= -1.1f && objects[0]->GetPosition().y <= -1.1f) {
		ZidaneMovePhase = 4;
	}
}

/*
This function controls the UI that is rendered to the screen. The main thing here will be a move counter,
	but other outputs such as data for the game may also be included here.
*/
void GameEngine::UI() {
	// Start a new frame.
	ImGui_ImplGlfwGL3_NewFrame();

	// Move counter UI box.
	{
		ImGui::Begin("Move Counter", &showMoveCounter);
		ImGui::Text("Moves taken: %d", movesTaken);
		ImGui::End();
	}

	// Debug window.
	if (displayDebug) {
		ImGui::Begin("Debug", &displayDebug);
		ImGui::Text("Current frame rate: %.3f", 10000.0f / ImGui::GetIO().Framerate); // Current frame rate.
		ImGui::End();
	}

	// Win window.
	if (win) {
		ImGui::Begin("Winner", &win);
		ImGui::Text("You win! This took you %d moves! Can you do it in fewer?", movesTaken);
		ImGui::End();
	}

	// Render the windows and draw them to the screen.
	ImGui::Render();
	ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
}

/*
This function controls the audio for the game. It takes in the file name, finds the audio track
	specified and loads it if it is not inside the game loop. If we are inside the game loop, then
	insted of loading the sound, we play it. 
*/
void GameEngine::Audio(FMOD::System* lowLevelSystem, FMOD::Sound* horn, FMOD::Channel* hornChannel) {
	if (playHorn) {
		// Play the horn sound when an illegal move is made.
		std::cout << "beep beep" << std::endl;
		lowLevelSystem->playSound(horn, NULL, false, &hornChannel);
		playHorn = false;
	}
}

/*
This function handles sending and receiving messages to and from the server.
*/
void GameEngine::Network(ENetHost* client, ENetEvent& enetEvent, ClientData* clientData, int& clientIndex, ENetPeer* peer, PhysicsData* receivedData) {
	// Generate a packet type.
	int* packetType = new int;
	*packetType = -1;
	while (enet_host_service(client, &enetEvent, 0) > 0) {
		switch (enetEvent.type) {
		case ENET_EVENT_TYPE_RECEIVE:
			// When the client receieves a packet...
			memcpy(packetType, enetEvent.packet->data, sizeof(int));
			if (*packetType == 0) {
				// ...Set the client index and client data.
				std::cout << "Packet recieved from server." << std::endl;
				memcpy(clientData, enetEvent.packet->data, sizeof(ClientData));
				clientIndex = clientData->clientIndex;
			}
			else if (*packetType == 1) {
				// ...Set the position of the same car moved on the other instance.
				memcpy(receivedData, enetEvent.packet->data, sizeof(PhysicsData));
				std::cout << receivedData->index << std::endl;
				objects[receivedData->index]->SetPosition(glm::vec3(receivedData->position.x, receivedData->position.y, receivedData->position.z) - objects[receivedData->index]->GetPosition());
				InitialiseGrid();
			}
			break;
		}
	}
}

/*
This sets the grid when the game starts before the game loop is run, and resets the grid 
	every time a car moves. The grid is used to keep track of where the cars are so that none
	of the cars can go inside another.
*/
void GameEngine::InitialiseGrid() {
	// Clear the grid.
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 5; j++) {
			grid[i][j] = 0;
		}
	}

	for (int i = 0; i < objects.size(); i++) {
		if (dynamic_cast<NormalCar*>(objects[i]) != nullptr) {
			NormalCar* tempCar = dynamic_cast<NormalCar*>(objects[i]);
			if (tempCar->HoriVert == 'H') {
				// Set the grid positions for the horizontal normal cars.
				int tempX = (tempCar->GetPosition().x + 0.625) * 4;
				int tempY = 4 - ((tempCar->GetPosition().y + 0.5) * 4);
				grid[tempX][tempY] = i;
				grid[tempX + 1][tempY] = i;
			}
			else if (tempCar->HoriVert == 'V') {
				// Set the grid positions for the vertical normal cars.
				int tempX = (tempCar->GetPosition().x + 0.75) * 4;
				int tempY = 4 - ((tempCar->GetPosition().y + 0.375) * 4);
				grid[tempX][tempY] = i;
				grid[tempX][tempY - 1] = i;
			}
		}
		else if (dynamic_cast<TargetCar*>(objects[i]) != nullptr) {
			// Set the grid position for the target car.
			int tempX = (objects[i]->GetPosition().x + 0.5) * 4;
			int tempY = 4 - ((objects[i]->GetPosition().y + 0.5) * 4);
			grid[tempX][tempY] = i;
			grid[tempX + 1][tempY] = i;
			grid[tempX + 2][tempY] = i;
		}
	}
}
