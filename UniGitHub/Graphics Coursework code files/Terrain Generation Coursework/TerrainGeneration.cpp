// TerrainGeneration.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <time.h>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normals;
};

const int nSize = 33;
float terrainGrid[nSize][nSize];
Vertex terrainVertices[nSize * nSize];
float hVal = 0.1f;
float randomNumber;
float randMax = 1.0f;
unsigned int vao, vbo, ebo;
const int verticesPerStrip = 2 * nSize;
const int numStripsRequired = nSize - 1;
unsigned int terrainIndexData[numStripsRequired][verticesPerStrip];

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window, glm::vec3* cameraPos, glm::vec3* cameraFront, glm::vec3* up) {
	const float cameraSpeed = 0.1f;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		(*cameraPos).z -= cameraSpeed;
		(*cameraFront).z -= cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		(*cameraPos).z += cameraSpeed;
		(*cameraFront).z += cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		(*cameraPos).x -= cameraSpeed;
		(*cameraFront).x -= cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		(*cameraPos).x += cameraSpeed;
		(*cameraFront).x += cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		(*cameraPos).y += cameraSpeed;
		(*cameraFront).y += cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		(*cameraPos).y -= cameraSpeed;
		(*cameraFront).y -= cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		(*cameraFront).z -= cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		(*cameraFront).z += cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		(*cameraFront).x -= cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		(*cameraFront).x += cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		(*cameraFront).y += cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		(*cameraFront).y -= cameraSpeed;
	}
}

void squareStep(int x, int y, int reach) {
	int count = 0;
	float average = 0.0f;
	if (x - reach >= 0 && y - reach >= 0) {
		average += terrainGrid[x - reach][y - reach];
		count++;
	}
	if (x - reach >= 0 && y + reach < nSize) {
		average += terrainGrid[x - reach][y + reach];
		count++;
	}
	if (x + reach < nSize && y - reach >= 0) {
		average += terrainGrid[x + reach][y - reach];
		count++;
	}
	if (x + reach < nSize && y + reach < nSize) {
		average += terrainGrid[x + reach][y + reach];
		count++;
	}
	average += ((((float)rand() / RAND_MAX) * 2) - 1) * randMax;
	average /= count;
	terrainGrid[x][y] = average;
}

void diamondStep(int x, int y, int reach) {
	int count = 0;
	float average = 0.0f;
	if (x - reach >= 0) {
		average += terrainGrid[x - reach][y];
		count++;
	}
	if (x + reach < nSize) {
		average += terrainGrid[x + reach][y];
		count++;
	}
	if (y - reach >= 0) {
		average += terrainGrid[x][y - reach];
		count++;
	}
	if (y + reach < nSize) {
		average += terrainGrid[x][y + reach];
		count++;
	}
	average += ((((float)rand() / RAND_MAX) * 2) - 1) * randMax;
	average /= count;
	terrainGrid[x][y] = average;
}

void diamondSquare(int size) {
	int sizeHalf = size / 2;
	if (sizeHalf < 1) {
		return;
	}

	// Square steps
	for (int y = sizeHalf; y < nSize; y += size) {
		for (int x = sizeHalf; x < nSize; x += size) {
			squareStep(x % nSize, y % nSize, sizeHalf);
		}
	}

	// Diamond steps
	int col = 0;
	for (int x = 0; x < nSize; x += sizeHalf)
	{
		col++;
		if (col % 2 == 1) {
			for (int y = sizeHalf; y < nSize; y += size) {
				diamondStep(x % nSize, y % nSize, sizeHalf);
			}
		}
		else {
			for (int y = 0; y < nSize; y += size) {
				diamondStep(x % nSize, y % nSize, sizeHalf);
			}
		}
	}
	randMax = randMax * glm::pow(2, -hVal);
	diamondSquare(size / 2);
}

void Draw(Shader& shaderProgram) {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-15.0f, -2.5f, -40.0f));
	int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	for (int i = 0; i < nSize - 1; i++)
	{
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "material.ambient"), 0.0f, 1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "material.diffuse"), 0.0f, 1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "material.specular"), 0.2f, 0.2f, 0.2f);
		shaderProgram.SetFloat("material.shiniess", 32);
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLE_STRIP, verticesPerStrip, GL_UNSIGNED_INT, terrainIndexData[i]);
		glBindVertexArray(0);
	}
}

void SetupMesh() {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(terrainVertices), terrainVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(terrainVertices[0]), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(terrainVertices[0]), (void*)sizeof(terrainVertices[0].position));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Render(GLFWwindow* window, Shader& shaderProgram, glm::vec3* cameraPos, glm::vec3* cameraFront, glm::vec3* up, glm::vec3 lightDirection, glm::vec3 lightDiffuse, glm::vec3 lightAmbient, glm::vec3 lightSpecular) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use shader program
	shaderProgram.Use();

	// use light
	shaderProgram.SetVec3("light.direction", lightDirection.x, lightDirection.y, lightDirection.z);
	shaderProgram.SetVec3("viewPos", (*cameraPos).x, (*cameraPos).y, (*cameraPos).z);
	shaderProgram.SetVec3("light.ambient", lightAmbient.x, lightAmbient.y, lightAmbient.z);
	shaderProgram.SetVec3("light.diffuse", lightDiffuse.x, lightDiffuse.y, lightDiffuse.z);
	shaderProgram.SetVec3("light.specular", lightSpecular.x, lightSpecular.y, lightSpecular.z);

	// set view and projection matrices. send view and projection matrices to shader and use them
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection;
	view = glm::lookAt(*cameraPos, *cameraFront, *up);
	projection = glm::perspective(glm::radians(45.0f), 800.0f / 800.0f, 0.1f, 100.0f);
	int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
	int projectionLoc = glGetUniformLocation(shaderProgram.ID, "projection");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// draw the terrain
	Draw(shaderProgram);

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void InitiateOpenGL() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 800, "TerrainGeneration", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window." << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialise GLAD." << std::endl;
	}

	glEnable(GL_DEPTH_TEST);

	Shader shaderProgram("VertexShader.txt", "FragmentShader.txt");

	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 lightDir = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 lightAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	glm::vec3 lightDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 lightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);

	for (int i = 0; i < nSize; i++) {
		for (int j = 0; j < nSize; j++) {
			terrainGrid[i][j] = 0;
		}
	}

	terrainGrid[0][0] = 0;
	terrainGrid[0][nSize] = 0;
	terrainGrid[nSize][nSize] = 0;
	terrainGrid[nSize][0] = 0;

	diamondSquare(nSize / 2);

	int index = 0;
	for (int i = 0; i < nSize; i++) {
		for (int j = 0; j < nSize; j++) {
			terrainVertices[index].position.x = i;
			terrainVertices[index].position.y = terrainGrid[i][j];
			terrainVertices[index].position.z = j;
			index++;
		}
	}

	index = 0;
	for (int z = 0; z < nSize - 1; z++)
	{
		index = z * nSize;
		for (int x = 0; x < nSize * 2; x += 2)
		{
			terrainIndexData[z][x] = index;
			index++;
		}
		for (int x = 1; x < nSize * 2 + 1; x += 2)
		{
			terrainIndexData[z][x] = index;
			index++;
		}
	}

	for (int i = 0; i < nSize * nSize; i++) {
		terrainVertices[i].normals = glm::vec3(0.0f, 0.0f, 1.0f);
	}

	SetupMesh();

	while (!glfwWindowShouldClose(window)) {
		Render(window, shaderProgram, &cameraPos, &cameraFront, &up, lightDir, lightDiffuse, lightAmbient, lightSpecular);

		processInput(window, &cameraPos, &cameraFront, &up);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}

int main()
{
	srand(time(NULL));
	InitiateOpenGL();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
