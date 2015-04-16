

#ifndef GameObject_H_
#define GameObject_H_

#include <vector>
#include "tiny_obj_loader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr

//#include "GameObjectSimplePhysics.h"
//#include "ObjectRenderer.h"

class GameObjectSimplePhysics;
class ObjectRenderer;

class GameObject {
public:

	GameObject(glm::vec3 position, GameObjectSimplePhysics *physics, ObjectRenderer *renderer);
	void update();
	void render();

	GameObjectSimplePhysics *getPhysics();
	glm::vec3 pos;
	glm::vec3 vel;
	float radius;
	float rotY;
	void setVelocity(glm::vec3 velocity);
	ObjectRenderer *getRenderer();
private:
	GameObjectSimplePhysics *physicsEngine;
	ObjectRenderer *objectRenderer;
};

#endif
