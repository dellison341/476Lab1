
#include "GameObjectSimplePhysics.h"


GameObjectSimplePhysics::GameObjectSimplePhysics() {

}

void GameObjectSimplePhysics::update(GameObject *gameObject /*and linkedlist here?*/) {
	// Will be moved to another class with a reference to this object as well as ther linked list
	gameObject->pos += gameObject->vel;
	/*
	while (linkedList != null) {
		if (node != this)
			if (node.pos.something too close to this,
				update velocity away from other object



	if (position on ground's border, still going towards border)
		velocity.x or .z = -velocity.x or .z
	*/
}
