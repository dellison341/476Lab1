// Program 4
// Cary Dobeck
// 2/27/2015

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cassert>
#include <cmath>
#include <stdio.h>
#include <list>
#include "GLSL.h"
#include "tiny_obj_loader.h"
#include "glm/glm.hpp"
#include "RenderingHelper.h"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr

#include "Camera.h"

#include "GameObject.h"
#include "GameObjectSimplePhysics.h"
#include "ObjectRenderer.h"
#include "PlayerPhysics.h"


#define NUM_BUNNIES 1
#define NUM_ROBOTS 10
#define SPEED 0.3
#define GROUND_EDGE 70
#define SPAWN_LIMIT 30

GLFWwindow* window;
using namespace std;

//vector<tinyobj::shape_t> shapes;
//vector<tinyobj::material_t> materials;

vector<tinyobj::shape_t> robotShapes;
vector<tinyobj::material_t> robotMaterials;

GameObject *player;
GameObjectSimplePhysics *physicsEngine;
ObjectRenderer *objectRenderer;

int g_SM = 1;
int g_width;
int g_height;
float g_angle = 0;
int g_mat_id = 0;
glm::vec3 g_trans(0, 0, -3.0f);
glm::vec3 g_light(0.0f, 40.0f, 0.0f);

float g_bunnyPositions[NUM_BUNNIES * 2];
float g_bunnyVelocities[NUM_BUNNIES * 2];
float g_bunnyRotations[NUM_BUNNIES];
int g_bunnyMesh[NUM_BUNNIES];

float g_robotPositions[NUM_ROBOTS * 2];
float g_robotVelocities[NUM_ROBOTS * 2];
float g_robotRotations[NUM_ROBOTS];
int g_robotMesh[NUM_ROBOTS];

glm::vec2 cameraRotate(0.0f, 0.0f);
glm::vec3 cameraZoom(0.0f, 0.0f, 0.0f);
GLint currentLMBstate = 0;
Camera currentCamera(3.0f);

GLuint ShadeProg;
//GLuint posBufObj = 0;
//GLuint norBufObj = 0;
//GLuint indBufObj = 0;

float drawNormals = -1.0;
GLint uDrawNormals;
float g_angleBunny = 0.0;
GLint uCameraLoc;

//all the game objects
vector<GameObject *> allObjects;


//Handles to the shader data
GLint h_aPosition;
GLint h_aNormal;
GLint h_uModelMatrix;
GLint h_uViewMatrix;
GLint h_uProjMatrix;
GLint h_uLightPos;
GLint h_uMatAmb, h_uMatDif, h_uMatSpec, h_uMatShine;
GLint h_uShadeM;

// For ground
GLuint posBufObjG = 0;
GLuint norBufObjG = 0;

// For Robot
GLuint posBufObjR = 0;
GLuint norBufObjR = 0;
GLuint indBufObjR = 0;


//declare a matrix stack
RenderingHelper ModelTrans;
float theta;
float thetaRotateBy = 0.5;


/* helper function to make sure your matrix handle is correct */
inline void safe_glUniformMatrix4fv(const GLint handle, const GLfloat data[]) {
	if (handle >= 0)
		glUniformMatrix4fv(handle, 1, GL_FALSE, data);
}

/* helper function to send materials to the shader - you must create your own */
void SetMaterial(int i) {
	glUseProgram(ShadeProg);
	switch (i) {
	case 0: //shiny blue plastic
		glUniform3f(h_uMatAmb, 0.02, 0.02, 0.1);
		glUniform3f(h_uMatDif, 0.0, 0.08, 0.5);
		glUniform3f(h_uMatSpec, 0.14, 0.14, 0.4);
		glUniform1f(h_uMatShine, 120.0);
		break;
	case 1: // flat grey
		glUniform3f(h_uMatAmb, 0.13, 0.13, 0.14);
		glUniform3f(h_uMatDif, 0.3, 0.3, 0.4);
		glUniform3f(h_uMatSpec, 0.3, 0.3, 0.4);
		glUniform1f(h_uMatShine, 4.0);
		break;
	case 2: //gold
		glUniform3f(h_uMatAmb, 0.09, 0.07, 0.08);
		glUniform3f(h_uMatDif, 0.91, 0.782, 0.82);
		glUniform3f(h_uMatSpec, 1.0, 0.913, 0.8);
		glUniform1f(h_uMatShine, 200.0);
		break;
	case 3: //something
		glUniform3f(h_uMatAmb, 0.09, 0.07, 0.08);
		glUniform3f(h_uMatDif, 0.31, 0.982, 0.12);
		glUniform3f(h_uMatSpec, 1.0, 0.613, 0.9);
		glUniform1f(h_uMatShine, 20.0);
		break;
	case 4: //something
		glUniform3f(h_uMatAmb, 0.09, 0.07, 0.08);
		glUniform3f(h_uMatDif, 0.41, 0.782, 0.82);
		glUniform3f(h_uMatSpec, 1.0, 0.913, 0.0);
		glUniform1f(h_uMatShine, 100.0);
		break;
	}
}

/* helper function to set projection matrix - don't touch */
void SetProjectionMatrix() {
	glm::mat4 Projection = glm::perspective(80.0f, (float)g_width / g_height, 0.1f, 100.f);
	safe_glUniformMatrix4fv(h_uProjMatrix, glm::value_ptr(Projection));
}

/* model transforms */
void SetModel() {
	glm::mat4 Trans = glm::translate(glm::mat4(1.0f), g_trans);
	glm::mat4 RotateY = glm::rotate(glm::mat4(1.0f), g_angle, glm::vec3(0.0f, 1, 0));
	glm::mat4 RotateX = glm::rotate(glm::mat4(1.0f), g_angleBunny, glm::vec3(1, 0, 0));
	glm::mat4 com = Trans * (RotateY * RotateX);
	safe_glUniformMatrix4fv(h_uModelMatrix, glm::value_ptr(com));
}

void drawBunny(int numBunny, GLsizei nIndices) {
	glm::mat4 Trans = glm::translate(glm::mat4(1.0f), glm::vec3(g_bunnyPositions[numBunny], 0, g_bunnyPositions[numBunny + NUM_BUNNIES]));
	glm::mat4 RotateY = glm::rotate(glm::mat4(1.0f), g_bunnyRotations[numBunny], glm::vec3(0.0f, 1, 0));
	glm::mat4 com = Trans * RotateY;
	safe_glUniformMatrix4fv(h_uModelMatrix, glm::value_ptr(com));
	SetMaterial(g_bunnyMesh[numBunny] % 5);
	glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
}

/* model transforms */
void SetLight() {
	glm::mat4 Trans = glm::translate(glm::mat4(1.0f), glm::vec3(g_light.x, g_light.y, g_light.z));
	glm::mat4 Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.05, 0.05, 0.05));
	glm::mat4 com = Trans * Scale;
	safe_glUniformMatrix4fv(h_uModelMatrix, glm::value_ptr(com));
}


//Given a vector of shapes which has already been read from an obj file
// resize all vertices to the range [-1, 1]
/*
void resize_obj(std::vector<tinyobj::shape_t> &shapes){
	float minX, minY, minZ;
	float maxX, maxY, maxZ;
	float scaleX, scaleY, scaleZ;
	float shiftX, shiftY, shiftZ;
	float epsilon = 0.001;

	minX = minY = minZ = 1.1754E+38F;
	maxX = maxY = maxZ = -1.1754E+38F;

	//Go through all vertices to determine min and max of each dimension
	for (size_t i = 0; i < shapes.size(); i++) {
		for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
			if (shapes[i].mesh.positions[3 * v + 0] < minX) minX = shapes[i].mesh.positions[3 * v + 0];
			if (shapes[i].mesh.positions[3 * v + 0] > maxX) maxX = shapes[i].mesh.positions[3 * v + 0];

			if (shapes[i].mesh.positions[3 * v + 1] < minY) minY = shapes[i].mesh.positions[3 * v + 1];
			if (shapes[i].mesh.positions[3 * v + 1] > maxY) maxY = shapes[i].mesh.positions[3 * v + 1];

			if (shapes[i].mesh.positions[3 * v + 2] < minZ) minZ = shapes[i].mesh.positions[3 * v + 2];
			if (shapes[i].mesh.positions[3 * v + 2] > maxZ) maxZ = shapes[i].mesh.positions[3 * v + 2];
		}
	}
	//From min and max compute necessary scale and shift for each dimension
	float maxExtent, xExtent, yExtent, zExtent;
	xExtent = maxX - minX;
	yExtent = maxY - minY;
	zExtent = maxZ - minZ;
	if (xExtent >= yExtent && xExtent >= zExtent) {
		maxExtent = xExtent;
	}
	if (yExtent >= xExtent && yExtent >= zExtent) {
		maxExtent = yExtent;
	}
	if (zExtent >= xExtent && zExtent >= yExtent) {
		maxExtent = zExtent;
	}
	scaleX = 2.0 / maxExtent;
	shiftX = minX + (xExtent / 2.0);
	scaleY = 2.0 / maxExtent;
	shiftY = minY + (yExtent / 2.0);
	scaleZ = 2.0 / maxExtent;
	shiftZ = minZ + (zExtent) / 2.0;

	//Go through all verticies shift and scale them
	for (size_t i = 0; i < shapes.size(); i++) {
		for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
			shapes[i].mesh.positions[3 * v + 0] = (shapes[i].mesh.positions[3 * v + 0] - shiftX) * scaleX;
			assert(shapes[i].mesh.positions[3 * v + 0] >= -1.0 - epsilon);
			assert(shapes[i].mesh.positions[3 * v + 0] <= 1.0 + epsilon);
			shapes[i].mesh.positions[3 * v + 1] = (shapes[i].mesh.positions[3 * v + 1] - shiftY) * scaleY;
			assert(shapes[i].mesh.positions[3 * v + 1] >= -1.0 - epsilon);
			assert(shapes[i].mesh.positions[3 * v + 1] <= 1.0 + epsilon);
			shapes[i].mesh.positions[3 * v + 2] = (shapes[i].mesh.positions[3 * v + 2] - shiftZ) * scaleZ;
			assert(shapes[i].mesh.positions[3 * v + 2] >= -1.0 - epsilon);
			assert(shapes[i].mesh.positions[3 * v + 2] <= 1.0 + epsilon);
		}
	}
}
*/


/*
void loadShapes(const string &objFile)
{
	string err = tinyobj::LoadObj(shapes, materials, objFile.c_str());
	if (!err.empty()) {
		cerr << err << endl;
	}
	resize_obj(shapes);
}
*/


void loadRobot(const string &objFile)
{
	string err = tinyobj::LoadObj(robotShapes, robotMaterials, objFile.c_str());
	if (!err.empty()) {
		cerr << err << endl;
	}
//	resize_obj(robotShapes);
}

void initGL()
{
	// Set the background color
	glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
	// Enable Z-buffer test
	glEnable(GL_DEPTH_TEST);
	glPointSize(18);

	//initialize the modeltrans matrix stack
	ModelTrans.useModelViewMatrix();
	ModelTrans.loadIdentity();
	theta = 0;
}

//get an x, y, z position that doesn't intersect with any other objects
glm::vec3 getNewPosition(vector<GameObject *> allObjects, float radius) {
	bool intersectsAnotherObject = false;
	vec3 result;
	
	do {
		result = vec3(rand() % (2 * SPAWN_LIMIT) - SPAWN_LIMIT,
					  0.0f,
					  rand() % (2 * SPAWN_LIMIT) - SPAWN_LIMIT);
		for(int i = 0; i < allObjects.size(); i++) {
			if (glm::distance(allObjects[i]->pos, result) < radius + allObjects[i]->radius) {
				intersectsAnotherObject = true;
			}
		}
	//	assert(result.x >= -200 && result.x <= 200);
	//	assert(result.z >= -200 && result.z <= 200);
	} while(intersectsAnotherObject);
	
	return result;
}

/*
void initBunnyShape() {
	// Send the position array to the GPU
	const vector<float> &posBuf = shapes[0].mesh.positions;
	glGenBuffers(1, &posBufObj);
	glBindBuffer(GL_ARRAY_BUFFER, posBufObj);
	glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW);

	// TODO compute the normals per vertex - you must fill this in 
	vector<float> norBuf;
	int idx1, idx2, idx3;
	glm::vec3 v1, v2, v3;
	//for every vertex initialize a normal to 0
	for (int j = 0; j < shapes[0].mesh.positions.size() / 3; j++) {
		norBuf.push_back(0);
		norBuf.push_back(0);
		norBuf.push_back(0);
	}
	// DO work here to compute the normals for every face
	//then add its normal to its associated vertex
	for (int i = 0; i < shapes[0].mesh.indices.size() / 3; i++) {
		idx1 = shapes[0].mesh.indices[3 * i + 0];
		idx2 = shapes[0].mesh.indices[3 * i + 1];
		idx3 = shapes[0].mesh.indices[3 * i + 2];
		v1 = glm::vec3(shapes[0].mesh.positions[3 * idx1 + 0], shapes[0].mesh.positions[3 * idx1 + 1], shapes[0].mesh.positions[3 * idx1 + 2]);
		v2 = glm::vec3(shapes[0].mesh.positions[3 * idx2 + 0], shapes[0].mesh.positions[3 * idx2 + 1], shapes[0].mesh.positions[3 * idx2 + 2]);
		v3 = glm::vec3(shapes[0].mesh.positions[3 * idx3 + 0], shapes[0].mesh.positions[3 * idx3 + 1], shapes[0].mesh.positions[3 * idx3 + 2]);

		v2 = v2 - v1;
		v3 = v3 - v1;
		glm::vec3 normal = glm::normalize(glm::cross(v2, v3));

		norBuf[3 * idx1 + 0] += normal.x;
		norBuf[3 * idx1 + 1] += normal.y;
		norBuf[3 * idx1 + 2] += normal.z;
		norBuf[3 * idx2 + 0] += normal.x;
		norBuf[3 * idx2 + 1] += normal.y;
		norBuf[3 * idx2 + 2] += normal.z;
		norBuf[3 * idx3 + 0] += normal.x;
		norBuf[3 * idx3 + 1] += normal.y;
		norBuf[3 * idx3 + 2] += normal.z;

	}

	// Normalize all the normals
	for (int i = 0; i < norBuf.size(); i += 3) {
		glm::vec3 newNormal = glm::normalize(glm::vec3(norBuf[i + 0], norBuf[i + 1], norBuf[i + 2]));
		norBuf[i + 0] = newNormal.x;
		norBuf[i + 1] = newNormal.y;
		norBuf[i + 2] = newNormal.z;
	}

	glGenBuffers(1, &norBufObj);
	glBindBuffer(GL_ARRAY_BUFFER, norBufObj);
	glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);

	// Send the index array to the GPU
	const vector<unsigned int> &indBuf = shapes[0].mesh.indices;
	glGenBuffers(1, &indBufObj);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBufObj);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indBuf.size()*sizeof(unsigned int), &indBuf[0], GL_STATIC_DRAW);

	// Unbind the arrays
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	GLSL::checkVersion();
	assert(glGetError() == GL_NO_ERROR);
}
*/

bool installShaders(const string &vShaderName, const string &fShaderName)
{
	GLint rc;

	// Create shader handles
	GLuint VS = glCreateShader(GL_VERTEX_SHADER);
	GLuint FS = glCreateShader(GL_FRAGMENT_SHADER);

	// Read shader sources
	const char *vshader = GLSL::textFileRead(vShaderName.c_str());
	const char *fshader = GLSL::textFileRead(fShaderName.c_str());
	glShaderSource(VS, 1, &vshader, NULL);
	glShaderSource(FS, 1, &fshader, NULL);

	// Compile vertex shader
	glCompileShader(VS);
	GLSL::printError();
	glGetShaderiv(VS, GL_COMPILE_STATUS, &rc);
	GLSL::printShaderInfoLog(VS);
	if (!rc) {
		printf("Error compiling vertex shader %s\n", vShaderName.c_str());
		return false;
	}

	// Compile fragment shader
	glCompileShader(FS);
	GLSL::printError();
	glGetShaderiv(FS, GL_COMPILE_STATUS, &rc);
	GLSL::printShaderInfoLog(FS);
	if (!rc) {
		printf("Error compiling fragment shader %s\n", fShaderName.c_str());
		return false;
	}

	// Create the program and link
	ShadeProg = glCreateProgram();
	glAttachShader(ShadeProg, VS);
	glAttachShader(ShadeProg, FS);
	glLinkProgram(ShadeProg);

	GLSL::printError();
	glGetProgramiv(ShadeProg, GL_LINK_STATUS, &rc);
	GLSL::printProgramInfoLog(ShadeProg);
	if (!rc) {
		printf("Error linking shaders %s and %s\n", vShaderName.c_str(), fShaderName.c_str());
		return false;
	}

	/* get handles to attribute data */
	h_aPosition = GLSL::getAttribLocation(ShadeProg, "aPosition");
	h_aNormal = GLSL::getAttribLocation(ShadeProg, "aNormal");
	h_uProjMatrix = GLSL::getUniformLocation(ShadeProg, "uProjMatrix");
	h_uViewMatrix = GLSL::getUniformLocation(ShadeProg, "uViewMatrix");
	h_uModelMatrix = GLSL::getUniformLocation(ShadeProg, "uModelMatrix");
	h_uLightPos = GLSL::getUniformLocation(ShadeProg, "uLightPos");
	h_uMatAmb = GLSL::getUniformLocation(ShadeProg, "UaColor");
	h_uMatDif = GLSL::getUniformLocation(ShadeProg, "UdColor");
	h_uMatSpec = GLSL::getUniformLocation(ShadeProg, "UsColor");
	h_uMatShine = GLSL::getUniformLocation(ShadeProg, "Ushine");
	h_uShadeM = GLSL::getUniformLocation(ShadeProg, "uShadeModel");
	uDrawNormals = GLSL::getUniformLocation(ShadeProg, "uDrawNormals");
	uCameraLoc = GLSL::getUniformLocation(ShadeProg, "uCameraLoc");

	assert(glGetError() == GL_NO_ERROR);
	return true;
}

void initBunnies() {
	for (int i = 0; i < NUM_BUNNIES; i++) {
		g_bunnyPositions[i] = (rand() / (float) RAND_MAX - 0.5f) * 40;
		g_bunnyPositions[i + NUM_BUNNIES] = (rand() / (float)RAND_MAX - 0.5f) * 40;
		g_bunnyRotations[i] = (rand() / (float)RAND_MAX - 0.5f) * 360;
		g_bunnyMesh[i] = rand() % 5;
	}
}
/*
void initRobots() {
	for (int i = 0; i < NUM_ROBOTS; i++) {
		g_robotPositions[i] = (rand() / (float)RAND_MAX - 0.5f) * 40;
		g_robotPositions[i * 2] = (rand() / (float)RAND_MAX - 0.5f) * 40;
		g_robotRotations[i] = (rand() / (float)RAND_MAX - 0.5f) * 360;
		g_robotMesh[i] = rand() % 5;
	}
}*/

void initGround() {


	GLfloat g_backgnd_data[] = {
		-GROUND_EDGE, -1.0f, -GROUND_EDGE,
		-GROUND_EDGE, -1.0f, GROUND_EDGE,
		GROUND_EDGE, -1.0f, -GROUND_EDGE,
		-GROUND_EDGE, -1.0f, GROUND_EDGE,
		GROUND_EDGE, -1.0f, -GROUND_EDGE,
		GROUND_EDGE, -1.0f, GROUND_EDGE,
	};

	GLfloat nor_Buf_G[] = {
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
	};

	glGenBuffers(1, &posBufObjG);
	glBindBuffer(GL_ARRAY_BUFFER, posBufObjG);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_backgnd_data), g_backgnd_data, GL_STATIC_DRAW);

	glGenBuffers(1, &norBufObjG);
	glBindBuffer(GL_ARRAY_BUFFER, norBufObjG);
	glBufferData(GL_ARRAY_BUFFER, sizeof(nor_Buf_G), nor_Buf_G, GL_STATIC_DRAW);
}

void initRobot() {
	// Send the position array to the GPU
	const vector<float> &posBuf = robotShapes[0].mesh.positions;
	glGenBuffers(1, &posBufObjR);
	glBindBuffer(GL_ARRAY_BUFFER, posBufObjR);
	glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW);

	// Send the normal array to the GPU
	const vector<float> &norBuf = robotShapes[0].mesh.normals;
	glGenBuffers(1, &norBufObjR);
	glBindBuffer(GL_ARRAY_BUFFER, norBufObjR);
	glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);

	// Send the index array to the GPU
	const vector<unsigned int> &indBuf = robotShapes[0].mesh.indices;
	glGenBuffers(1, &indBufObjR);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBufObjR);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indBuf.size()*sizeof(unsigned int), &indBuf[0], GL_STATIC_DRAW);

	// Unbind the arrays
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	GLSL::checkVersion();
	assert(glGetError() == GL_NO_ERROR);
}

void drawRobot(float translateX, float translateZ, float rotateY)
{

	// Enable and bind position array for drawing
	GLSL::enableVertexAttribArray(h_aPosition);
	glBindBuffer(GL_ARRAY_BUFFER, posBufObjR);
	glVertexAttribPointer(h_aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Enable and bind normal array for drawing
	GLSL::enableVertexAttribArray(h_aNormal);
	glBindBuffer(GL_ARRAY_BUFFER, norBufObjR);
	glVertexAttribPointer(h_aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Bind index array for drawing
	int nIndices = (int)robotShapes[0].mesh.indices.size();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBufObjR);

	// Compute and send the projection matrix - leave this as is
//	glm::mat4 Projection = glm::perspective(80.0f, (float)g_width / g_height, 0.1f, 100.f);
//	glUniformMatrix4fv(uP, 1, GL_FALSE, glm::value_ptr(Projection));

	//create the model transforms 
	ModelTrans.loadIdentity();
	// Torso
	ModelTrans.translate(glm::vec3(translateX, 0, translateZ));
	ModelTrans.rotate(rotateY, glm::vec3(0, 1, 0));
	ModelTrans.pushMatrix();
	// Upper left arm
	ModelTrans.translate(glm::vec3(0.20, 0.5, 0));
	ModelTrans.rotate(theta, glm::vec3(0, 0, 1));
	ModelTrans.translate(glm::vec3(0.45, 0, 0));

	ModelTrans.pushMatrix();
	// lower left arm
	ModelTrans.translate(glm::vec3(0.30, 0, 0));
	ModelTrans.rotate(theta, glm::vec3(0, 0, 1));
	ModelTrans.translate(glm::vec3(0.45, 0, 0));

	ModelTrans.pushMatrix();
	// lower left palm
	ModelTrans.translate(glm::vec3(0.50, 0, 0));

	ModelTrans.pushMatrix();
	// pinky finger
	ModelTrans.translate(glm::vec3(0.20, -0.1, 0));
	ModelTrans.scale(0.2, 0.1, 0.1);
	glUniformMatrix4fv(h_uModelMatrix, 1, GL_FALSE, glm::value_ptr(ModelTrans.modelViewMatrix));
	glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
	ModelTrans.popMatrix();

	ModelTrans.pushMatrix();
	// index finger
	ModelTrans.translate(glm::vec3(0.20, 0.1, 0));
	ModelTrans.scale(0.2, 0.1, 0.1);
	glUniformMatrix4fv(h_uModelMatrix, 1, GL_FALSE, glm::value_ptr(ModelTrans.modelViewMatrix));
	glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
	ModelTrans.popMatrix();

	ModelTrans.pushMatrix();
	// thunb
	ModelTrans.translate(glm::vec3(0, 0.20, 0));
	ModelTrans.scale(0.1, 0.2, 0.1);
	glUniformMatrix4fv(h_uModelMatrix, 1, GL_FALSE, glm::value_ptr(ModelTrans.modelViewMatrix));
	glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
	ModelTrans.popMatrix();

	ModelTrans.scale(0.2, 0.2, 0.1);
	glUniformMatrix4fv(h_uModelMatrix, 1, GL_FALSE, glm::value_ptr(ModelTrans.modelViewMatrix));
	glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
	ModelTrans.popMatrix();

	ModelTrans.scale(0.7, 0.2, 0.2);
	glUniformMatrix4fv(h_uModelMatrix, 1, GL_FALSE, glm::value_ptr(ModelTrans.modelViewMatrix));
	glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
	ModelTrans.popMatrix();

	ModelTrans.scale(0.7, 0.2, 0.2);

	glUniformMatrix4fv(h_uModelMatrix, 1, GL_FALSE, glm::value_ptr(ModelTrans.modelViewMatrix));
	glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
	ModelTrans.popMatrix();

	ModelTrans.pushMatrix();
	// Head
	ModelTrans.rotate(theta / 10, glm::vec3(0, 0, 1));
	ModelTrans.translate(glm::vec3(0, 0.8, 0));
	ModelTrans.scale(0.3, 0.3, 0.3);
	glUniformMatrix4fv(h_uModelMatrix, 1, GL_FALSE, glm::value_ptr(ModelTrans.modelViewMatrix));
	glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
	ModelTrans.popMatrix();

	ModelTrans.scale(0.5, 1.0, 0.5);
	glUniformMatrix4fv(h_uModelMatrix, 1, GL_FALSE, glm::value_ptr(ModelTrans.modelViewMatrix));
	glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
}

void moveAllObjects() {
	static float moveTime = 0.0;
	moveTime += 0.005;

	g_robotVelocities[0] = glm::cos(moveTime) * 10;
	g_robotVelocities[NUM_ROBOTS] = 0.01;

	g_robotPositions[0] = g_robotVelocities[0];
	g_robotPositions[NUM_ROBOTS] += g_robotVelocities[NUM_ROBOTS];


	for (int i = 0; i < NUM_ROBOTS; i++) {
		g_robotRotations[i] = 0; // glm::atan(g_robotVelocities[NUM_ROBOTS] / g_robotVelocities[0]) * 50;// / g_robotVelocities[NUM_ROBOTS] / g_robotVelocities[0] * 50;
	}

	for (int i = 0; i < NUM_BUNNIES; i++) {
		glm::vec2 velocity = glm::vec2(g_robotPositions[0] - g_bunnyPositions[i], g_robotPositions[NUM_ROBOTS] - g_bunnyPositions[i + NUM_BUNNIES]);
		velocity = velocity * 0.005f;
		g_bunnyVelocities[i] = velocity.x;
		g_bunnyVelocities[i + NUM_BUNNIES] = velocity.y;

		for (int j = i - 1; j >= 0; j--) {
			float differenceX = g_bunnyPositions[i] - g_bunnyPositions[j];
			if (-1 < differenceX && differenceX < 1) {
				g_bunnyVelocities[i] += differenceX * 0.03f;
				g_bunnyVelocities[j] -= differenceX * 0.03f;
			}

			float differenceY = g_bunnyPositions[i + NUM_BUNNIES] - g_bunnyPositions[j + NUM_BUNNIES];
			if (-1 < differenceY && differenceY < 1) {
				g_bunnyVelocities[i + NUM_BUNNIES] += differenceY * 0.03f;
				g_bunnyVelocities[j + NUM_BUNNIES] -= differenceY * 0.03f;
			}
		}
	}

	for (int i = 0; i < NUM_BUNNIES; i++) {
		g_bunnyPositions[i] += g_bunnyVelocities[i];
		g_bunnyPositions[i + NUM_BUNNIES] += g_bunnyVelocities[i + NUM_BUNNIES];
		g_bunnyRotations[i] = 0; // glm::atan(g_bunnyVelocities[NUM_BUNNIES] / g_bunnyVelocities[0]) * 10;
	}
}


void drawGL()
{
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our GLSL program
	glUseProgram(ShadeProg);

	SetProjectionMatrix();
	//get V matrix from camera
	safe_glUniformMatrix4fv(h_uViewMatrix, glm::value_ptr(currentCamera.getViewMatrix()));
	
	

	glUniform3f(h_uLightPos, g_light.x, g_light.y, g_light.z);
	glUniform1i(h_uShadeM, g_SM);
	glUniform1f(uDrawNormals, drawNormals);
	glUniform3f(uCameraLoc, 0, 0, -10);
	/*
	// Enable and bind position array for drawing
	GLSL::enableVertexAttribArray(h_aPosition);
	glBindBuffer(GL_ARRAY_BUFFER, posBufObj);
	glVertexAttribPointer(h_aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Enable and bind normal array for drawing
	GLSL::enableVertexAttribArray(h_aNormal);
	glBindBuffer(GL_ARRAY_BUFFER, norBufObj);
	glVertexAttribPointer(h_aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Bind index array for drawing
	int nIndices = (int)shapes[0].mesh.indices.size();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBufObj);
	*/


//	for (int i = 0; i < NUM_BUNNIES; i++) {
//		drawBunny(i, nIndices);
//	}
	
	for (int i = 0; i < allObjects.size(); i++) {
		allObjects[i]->getRenderer()->begin();
		allObjects[i]->render();
		allObjects[i]->getRenderer()->end();
	}

	objectRenderer->begin();
	player->render();
	objectRenderer->end();

	if (theta >= 45 || theta <= -45) {
		thetaRotateBy = -thetaRotateBy;
	}
	theta += thetaRotateBy;

	for (int i = 0; i < NUM_ROBOTS; i++) {
		SetMaterial(g_robotMesh[i]);
		drawRobot(g_robotPositions[i], g_robotPositions[i + NUM_ROBOTS], g_robotRotations[i]);
	}

	SetMaterial(3);

	//draw the ground
	glEnableVertexAttribArray(h_aPosition);
	glBindBuffer(GL_ARRAY_BUFFER, posBufObjG);
	glVertexAttribPointer(h_aPosition, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	GLSL::enableVertexAttribArray(h_aNormal);
	glBindBuffer(GL_ARRAY_BUFFER, norBufObjG);
	glVertexAttribPointer(h_aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	// end
	GLSL::disableVertexAttribArray(h_aPosition);
	GLSL::disableVertexAttribArray(h_aNormal);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Disable and unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);
	assert(glGetError() == GL_NO_ERROR);

}


void window_size_callback(GLFWwindow* window, int w, int h){
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	g_width = w;
	g_height = h;
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
	cameraRotate.x += yOffset * 0.1;
	cameraRotate.x = glm::max(glm::min(cameraRotate.x, 1.0f), -1.0f);
	cameraRotate.y += xOffset * 0.1;
}

void onCursorPosChange(GLFWwindow *window, double newX, double newY) {
	static double lastMouseX;
	static double lastMouseY;
	if (currentLMBstate == GLFW_PRESS) {
		//update phi and theta
		currentCamera.updateAngle(newX - lastMouseX, newY - lastMouseY);
		
		lastMouseY = newY;
		lastMouseX = newX;
	}
}

void onMouseBtnEvent(GLFWwindow *window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		currentLMBstate = action;
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_X && action == GLFW_PRESS)
		g_mat_id = (g_mat_id + 1) % 5;
	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
		g_SM = !g_SM;
	if (key == GLFW_KEY_N && action == GLFW_PRESS)
		drawNormals = -drawNormals;
}

int main(int argc, char **argv)
{

	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	// Open a window and create its OpenGL context
	g_width = 1024;
	g_height = 768;
	window = glfwCreateWindow(g_width, g_height, "P3 - shading", NULL, NULL);
	if (window == NULL){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetScrollCallback(window, scroll_callback);
	
	//cursor and mouse handlers
	glfwSetCursorPosCallback(window, onCursorPosChange);
	glfwSetMouseButtonCallback(window, onMouseBtnEvent);
	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//	loadrobot("sphere.obj");
	loadRobot("models/cube.obj");
	initGL();
//	initBunnyShape();
	initRobot();
	//initRobots();
	installShaders("shaders/vert.glsl", "shaders/frag.glsl");
	//currentCamera = new Camera(glm::vec3(0, 0, 0));
	initBunnies();
	initGround();

	glClearColor(0.6f, 0.6f, 0.8f, 1.0f);



	
	ObjectRenderer *tempRenderer;
	
	for (int i = 0; i < NUM_ROBOTS; i++) {
		tempRenderer = new ObjectRenderer("models/face.obj", h_uModelMatrix, h_aPosition, h_aNormal);
		physicsEngine = new GameObjectSimplePhysics();
		allObjects.push_back(new GameObject(getNewPosition(allObjects, tempRenderer->getRadius()), physicsEngine, tempRenderer));
	}
	
	physicsEngine = new GameObjectSimplePhysics();
	objectRenderer = new ObjectRenderer("models/bunny.obj", h_uModelMatrix, h_aPosition, h_aNormal);
	player = new GameObject(glm::vec3(0, 0, 0), physicsEngine, objectRenderer);
	allObjects.push_back(player);
	
	
	double lastTime = glfwGetTime();
	int nbFrames = 0;

	do{
		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0){ // If last prinf() was more than 1 sec ago
			// printf and reset timer
//			printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			printf("%f frames per second\n", nbFrames / (currentTime - lastTime));
			nbFrames = 0;
			lastTime += 1.0;
		}


		for (int i = 0; i < allObjects.size(); i++) {
			allObjects[i]->update();
		}
		//player->update();
		drawGL();
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
		
		currentCamera.setFocus(player->pos);
		glm::vec3 movement = glm::normalize(player->pos - currentCamera.getEyePosition());
		printf("Camera to focus vector is <%f, %f, %f>\n",
			   movement.x, movement.y, movement.z);

		GLint wState = glfwGetKey(window, GLFW_KEY_W);
		GLint sState = glfwGetKey(window, GLFW_KEY_S);
		
		if (wState == GLFW_PRESS) {
			movement *= SPEED;
		}
		else if (sState == GLFW_PRESS) {
			movement *= SPEED;
			movement *= -1;
		}
		else {
			movement = vec3(0.0);
		}
		movement.y = 0.0;
		player->setVelocity(movement);
		
		
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			g_light.x += 0.15;
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			g_light.x -= 0.15;
		
			


	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(window) == 0);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
