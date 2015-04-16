
#include "GameObject.h"

#include "GameObjectSimplePhysics.h"
#include "ObjectRenderer.h"


GameObject::GameObject(glm::vec3 position, GameObjectSimplePhysics *physics, ObjectRenderer *renderer) {

	pos = position;
	radius = renderer->getRadius();
	vel = glm::vec3(0.01f, 0, 0.01f);
	material = 4;
	physicsEngine = physics;
	objectRenderer = renderer;
}

void GameObject::update() {
	physicsEngine->update(this);
}

void GameObject::render() {
	objectRenderer->render(this);
}

GameObjectSimplePhysics * GameObject::getPhysics() {
	return physicsEngine;
}

ObjectRenderer * GameObject::getRenderer() {
	return objectRenderer;
}

void GameObject::setVelocity(glm::vec3 velocity) {
	vel = velocity;
}

void GameObject::setMaterial(int mat) {
	material = mat;
}