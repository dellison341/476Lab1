//
//  Camera.h specifies a Camera class, to be used
//  to compute view and projection information
//  for the game engine. This should be pretty easy
//  to hold multiple cameras (i.e. a debug camera).

#ifndef __Project__camera__
#define __Project__camera__

#include <stdio.h>
#include "glm/glm.hpp"
#include <GL/glew.h>


class Camera {
public:
	Camera(float cameraDistance);
	void updateFocus(glm::vec3 posChange);
	void setFocus(glm::vec3 newFocus);
	//updatePos takes the change in mouse position
	void updateAngle(double dx, double dy);
	glm::mat4 getViewMatrix();
	glm::vec3 getEyePosition();
private:
	glm::vec3 eye;
	glm::vec3 focusPos = glm::vec3(0, 0, 0);
	//pitch (up and down) is phi, yaw (side to side) is theta
	float phi = 0;
	float theta = 0;
	void updateEye();
	float radius;
};


#endif /* defined(__Project__camera__) */
