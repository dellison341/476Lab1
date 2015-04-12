//
//  camera.cpp
//  Project
//
//  Created by David Ellison on 4/12/15.
//
//

#include "camera.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

const float maxAngle = 3.14/2;
const float minAngle = -3.14/2;


Camera::Camera(glm::vec3 initialPosition) {
	
}

void Camera::updateFocus(glm::vec3 posChange) {
	
}

glm::mat4 Camera::getViewMatrix() {
	updateEye();
	glm::mat4 V = glm::lookAt(eye, glm::vec3(0,0,0), glm::vec3(0, 1, 0));
	static bool doShowOnce = true;
	if (doShowOnce) {
		for (int i = 0; i < 16; i++) {
			printf("lookAt[%d] is %f\n", i, glm::value_ptr(V)[i]);
		}
		doShowOnce = false;
	}
	//V *=
	return V;
}


void Camera::updateEye() {
	eye = glm::vec3(cos(phi)*cos(theta), sin(phi), cos(phi)*cos(3.14 / 2.0 - theta));
	
}

void Camera::updateAngle(double dx, double dy) {
	theta += (float)(dx / 100);
	phi += (float)(dy / 100);
	
	if (phi > maxAngle) {
		phi = maxAngle;
	}
	if (phi < minAngle) {
		phi = minAngle;
	}
	
	updateEye();
}

