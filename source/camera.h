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
		Camera(glm::vec3 initialPosition);
		void updateFocus(glm::vec3 posChange);
		//updatePos takes the change in mouse position
		void updateAngle(double dx, double dy);
		glm::mat4 getViewMatrix();
	private:
		glm::vec3 eye;
		glm::vec3 position;
		glm::vec3 view;
		glm::vec3 up;
		GLint viewMatrixHandle;
	    glm::vec2 cameraRotate;
		glm::vec3 cameraZoom;
		//pitch (up and down) is phi, yaw (side to side) is theta
		float phi = 0;
		float theta = 0;
		void updateEye();
};


#endif /* defined(__Project__camera__) */
