
#include "GameObject.h"

#include "GameObjectSimplePhysics.h"
#include "ObjectRenderer.h"


GameObject::GameObject(glm::vec3 position, GameObjectSimplePhysics *physics, ObjectRenderer *renderer) {

	pos = position;
	vel = glm::vec3(0.01f, 0, 0.01f);
	physicsEngine = physics;
	objectRenderer = renderer;
}

void GameObject::update() {
	physicsEngine->update(this);
}

void GameObject::render() {
	objectRenderer->render(this);
}