
#ifndef ObjectRenderer_H_
#define ObjectRenderer_H_

#include <vector>
#include "tiny_obj_loader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "GLSL.h"
#include <iostream>
#include <string>
#include "GameObject.h"


class ObjectRenderer {
public:
	ObjectRenderer(char *file, GLint modelMatrixHandle, GLint positionHandle, GLint normalHandle);
	void render(GameObject *gameObject);
	void end();
	void loadObjShapes(const std::string &objFile);
	void resizeObj(std::vector<tinyobj::shape_t> &shapes);
	void initObjShape();
	void setMaterialHandles(GLint matAmbHandle, GLint matDifHandle, GLint matSpecHandle, GLint matShineHandle);
	void setMaterial(int i);
	void init();
	void begin();
	float getRadius();

private:
	GLuint posBufObject;
	GLuint norBufObject;
	GLuint indBufObject;

	GLint h_uMatAmbHandle;
	GLint h_uMatDifHandle;
	GLint h_uMatSpecHandle;
	GLint h_uMatShineHandle;
	
	std::vector<tinyobj::shape_t> objectShapes;
	std::vector<tinyobj::material_t> objectMaterials;
	
	GLint h_uModelMatrixHandle;
	GLint h_aPositionHandle;
	GLint h_aNormalHandle;
	
	char *filePath;
	
	float boundRadius;

};

#endif
