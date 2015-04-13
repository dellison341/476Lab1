/*
 * RenderingHelper.h
 *
 *  Created on: Jul 28, 2011
 *      Author: Wyatt and Evan
 */

#ifndef GameObject_H_
#define GameObject_H_

#include <vector>
#include "tiny_obj_loader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class GameObject {
public:
	GameObject(std::vector<tinyobj::shape_t> *objShapes, GLint modelMatrixHandle, glm::vec3 position);
	void update();
	void render();
};

#endif
