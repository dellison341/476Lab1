
#include "GameObject.h"

std::vector<tinyobj::shape_t> *shape;
GLint h_uModelMatrixHandle;

glm::vec3 pos;
glm::vec3 vel;
float rotY;

GameObject::GameObject(std::vector<tinyobj::shape_t> *objShapes, GLint modelMatrixHandle, glm::vec3 position) {

	shape = objShapes;
	h_uModelMatrixHandle = modelMatrixHandle;
	pos = position;
	vel = glm::vec3(0.01f, 0, 0.01f);
}

void GameObject::update(/*LinkedList*/) {
	// Will be moved to another class with a reference to this object as well as ther linked list
	pos += vel;
	/*
	while (linkedList != null) {
		if (node != this)
			if (node.pos.something too close to this,
				update velocity away from other object



	if (position on ground's border, still going towards border)
		velocity.x or .z = -velocity.x or .z
	*/
}

void GameObject::render() {
	// Will make another class that will house all data on how to render object, have link to this object and other necessary data
	int nIndices = (int)(*shape)[0].mesh.indices.size();
	glm::mat4 Trans = glm::translate(glm::mat4(1.0f), pos);
	glm::mat4 RotateY = glm::rotate(glm::mat4(1.0f), rotY, glm::vec3(0.0f, 1, 0));
	glm::mat4 com = Trans * RotateY;
//	safe_glUniformMatrix4fv(h_uModelMatrix, glm::value_ptr(com));
	glUniformMatrix4fv(h_uModelMatrixHandle, 1, GL_FALSE, glm::value_ptr(com));
//	SetMaterial(g_bunnyMesh[numBunny] % 5);
	glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
}