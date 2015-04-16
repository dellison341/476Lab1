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


Camera::Camera(float cameraDistance) {
	radius = cameraDistance;
}

void Camera::updateFocus(glm::vec3 posChange) {
	focusPos += posChange;
	printf("focusPos is now <%f, %f, %f>\n", focusPos.x, focusPos.y, focusPos.z);
}

void Camera::setFocus(glm::vec3 newFocus) {
	focusPos = newFocus;
}

glm::mat4 Camera::getViewMatrix() {
	updateEye();
	glm::mat4 V = glm::lookAt(eye, focusPos, glm::vec3(0, 1, 0));

	return V;
}


void Camera::updateEye() {
	eye = glm::vec3(cos(phi)*cos(theta), sin(phi), cos(phi)*cos(3.14 / 2.0 - theta));
	eye *= radius;
	eye += focusPos;
}

glm::vec3 Camera::getEyePosition() {
	return glm::vec3(eye);
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
	
}

