/*
 * RenderingHelper.h
 *
 *  Created on: Jul 28, 2011
 *      Author: Wyatt and Evan
 */

#ifndef GameObjectSimplePhysics_H_
#define GameObjectSimplePhysics_H_

#include <vector>
#include "tiny_obj_loader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "GameObject.h"

class GameObjectSimplePhysics {
public:
	GameObjectSimplePhysics();
	void update(GameObject *gameObject);
};

#endif
