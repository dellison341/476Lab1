
#include "ObjectRenderer.h"



ObjectRenderer::ObjectRenderer(char *file, GLint modelMatrixHandle, GLint positionHandle, GLint normalHandle) {
	h_uModelMatrixHandle = modelMatrixHandle;
	h_aPositionHandle = positionHandle;
	h_aNormalHandle = normalHandle;
	h_uMatAmbHandle = 0;
	h_uMatDifHandle = 0;
	h_uMatShineHandle = 0;
	h_uMatSpecHandle = 0;
	posBufObject = 0;
	indBufObject = 0;
	norBufObject = 0;
	filePath = file;
	init();
}

void ObjectRenderer::init() {
	loadObjShapes(filePath);
	resizeObj(objectShapes);
	initObjShape();
}

void ObjectRenderer::begin() {
	// Enable and bind position array for drawing
	GLSL::enableVertexAttribArray(h_aPositionHandle);
	glBindBuffer(GL_ARRAY_BUFFER, posBufObject);
	glVertexAttribPointer(h_aPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Enable and bind normal array for drawing
	GLSL::enableVertexAttribArray(h_aNormalHandle);
	glBindBuffer(GL_ARRAY_BUFFER, norBufObject);
	glVertexAttribPointer(h_aNormalHandle, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Bind index array for drawing
	int nIndices = (int)objectShapes[0].mesh.indices.size();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBufObject);
}

void ObjectRenderer::render(GameObject *gameObject) {
	setMaterial(gameObject->material);
	int nIndices = (int)(objectShapes)[0].mesh.indices.size();
	glm::mat4 Trans = glm::translate(glm::mat4(1.0f), gameObject->pos);
	glm::mat4 RotateY = glm::rotate(glm::mat4(1.0f), gameObject->rotY, glm::vec3(0.0f, 1, 0));
	glm::mat4 com = Trans * RotateY;
//	safe_glUniformMatrix4fv(h_uModelMatrix, glm::value_ptr(com));
	glUniformMatrix4fv(h_uModelMatrixHandle, 1, GL_FALSE, glm::value_ptr(com));
//	SetMaterial(g_bunnyMesh[numBunny] % 5);
	glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
}

void ObjectRenderer::end() {

}

void ObjectRenderer::initObjShape() {
	// Send the position array to the GPU
	const std::vector<float> &posBuf = objectShapes[0].mesh.positions;
	glGenBuffers(1, &posBufObject);
	glBindBuffer(GL_ARRAY_BUFFER, posBufObject);
	glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW);

	// TODO compute the normals per vertex - you must fill this in 
	std::vector<float> norBuf;
	int idx1, idx2, idx3;
	glm::vec3 v1, v2, v3;
	//for every vertex initialize a normal to 0
	for (int j = 0; j < objectShapes[0].mesh.positions.size() / 3; j++) {
		norBuf.push_back(0);
		norBuf.push_back(0);
		norBuf.push_back(0);
	}
	// DO work here to compute the normals for every face
	//then add its normal to its associated vertex
	for (int i = 0; i < objectShapes[0].mesh.indices.size() / 3; i++) {
		idx1 = objectShapes[0].mesh.indices[3 * i + 0];
		idx2 = objectShapes[0].mesh.indices[3 * i + 1];
		idx3 = objectShapes[0].mesh.indices[3 * i + 2];
		v1 = glm::vec3(objectShapes[0].mesh.positions[3 * idx1 + 0], objectShapes[0].mesh.positions[3 * idx1 + 1], objectShapes[0].mesh.positions[3 * idx1 + 2]);
		v2 = glm::vec3(objectShapes[0].mesh.positions[3 * idx2 + 0], objectShapes[0].mesh.positions[3 * idx2 + 1], objectShapes[0].mesh.positions[3 * idx2 + 2]);
		v3 = glm::vec3(objectShapes[0].mesh.positions[3 * idx3 + 0], objectShapes[0].mesh.positions[3 * idx3 + 1], objectShapes[0].mesh.positions[3 * idx3 + 2]);

		v2 = v2 - v1;
		v3 = v3 - v1;
		glm::vec3 normal = glm::normalize(glm::cross(v2, v3));

		norBuf[3 * idx1 + 0] += normal.x;
		norBuf[3 * idx1 + 1] += normal.y;
		norBuf[3 * idx1 + 2] += normal.z;
		norBuf[3 * idx2 + 0] += normal.x;
		norBuf[3 * idx2 + 1] += normal.y;
		norBuf[3 * idx2 + 2] += normal.z;
		norBuf[3 * idx3 + 0] += normal.x;
		norBuf[3 * idx3 + 1] += normal.y;
		norBuf[3 * idx3 + 2] += normal.z;

	}

	// Normalize all the normals
	for (int i = 0; i < norBuf.size(); i += 3) {
		glm::vec3 newNormal = glm::normalize(glm::vec3(norBuf[i + 0], norBuf[i + 1], norBuf[i + 2]));
		norBuf[i + 0] = newNormal.x;
		norBuf[i + 1] = newNormal.y;
		norBuf[i + 2] = newNormal.z;
	}

	glGenBuffers(1, &norBufObject);
	glBindBuffer(GL_ARRAY_BUFFER, norBufObject);
	glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);

	// Send the index array to the GPU
	const std::vector<unsigned int> &indBuf = objectShapes[0].mesh.indices;
	glGenBuffers(1, &indBufObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBufObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indBuf.size()*sizeof(unsigned int), &indBuf[0], GL_STATIC_DRAW);

	// Unbind the arrays
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	GLSL::checkVersion();
	assert(glGetError() == GL_NO_ERROR);
}

//Given a vector of shapes which has already been read from an obj file
// resize all vertices to the range [-1, 1]
void ObjectRenderer::resizeObj(std::vector<tinyobj::shape_t> &shapes){
	float minX, minY, minZ;
	float maxX, maxY, maxZ;
	float scaleX, scaleY, scaleZ;
	float shiftX, shiftY, shiftZ;
	float epsilon = 0.001;
	
	boundRadius = 0.0f;
	float maxSq = 0.0f;
	float curSq;

	minX = minY = minZ = 1.1754E+38F;
	maxX = maxY = maxZ = -1.1754E+38F;

	//Go through all vertices to determine min and max of each dimension
	for (size_t i = 0; i < shapes.size(); i++) {
		for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
			if (shapes[i].mesh.positions[3 * v + 0] < minX) minX = shapes[i].mesh.positions[3 * v + 0];
			if (shapes[i].mesh.positions[3 * v + 0] > maxX) maxX = shapes[i].mesh.positions[3 * v + 0];

			if (shapes[i].mesh.positions[3 * v + 1] < minY) minY = shapes[i].mesh.positions[3 * v + 1];
			if (shapes[i].mesh.positions[3 * v + 1] > maxY) maxY = shapes[i].mesh.positions[3 * v + 1];

			if (shapes[i].mesh.positions[3 * v + 2] < minZ) minZ = shapes[i].mesh.positions[3 * v + 2];
			if (shapes[i].mesh.positions[3 * v + 2] > maxZ) maxZ = shapes[i].mesh.positions[3 * v + 2];
			

		}
	}
	//From min and max compute necessary scale and shift for each dimension
	float maxExtent, xExtent, yExtent, zExtent;
	xExtent = maxX - minX;
	yExtent = maxY - minY;
	zExtent = maxZ - minZ;
	if (xExtent >= yExtent && xExtent >= zExtent) {
		maxExtent = xExtent;
	}
	if (yExtent >= xExtent && yExtent >= zExtent) {
		maxExtent = yExtent;
	}
	if (zExtent >= xExtent && zExtent >= yExtent) {
		maxExtent = zExtent;
	}
	scaleX = 2.0 / maxExtent;
	shiftX = minX + (xExtent / 2.0);
	scaleY = 2.0 / maxExtent;
	shiftY = minY + (yExtent / 2.0);
	scaleZ = 2.0 / maxExtent;
	shiftZ = minZ + (zExtent) / 2.0;

	//Go through all verticies shift and scale them
	for (size_t i = 0; i < shapes.size(); i++) {
		for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
			shapes[i].mesh.positions[3 * v + 0] = (shapes[i].mesh.positions[3 * v + 0] - shiftX) * scaleX;
			assert(shapes[i].mesh.positions[3 * v + 0] >= -1.0 - epsilon);
			assert(shapes[i].mesh.positions[3 * v + 0] <= 1.0 + epsilon);
			shapes[i].mesh.positions[3 * v + 1] = (shapes[i].mesh.positions[3 * v + 1] - shiftY) * scaleY;
			assert(shapes[i].mesh.positions[3 * v + 1] >= -1.0 - epsilon);
			assert(shapes[i].mesh.positions[3 * v + 1] <= 1.0 + epsilon);
			shapes[i].mesh.positions[3 * v + 2] = (shapes[i].mesh.positions[3 * v + 2] - shiftZ) * scaleZ;
			assert(shapes[i].mesh.positions[3 * v + 2] >= -1.0 - epsilon);
			assert(shapes[i].mesh.positions[3 * v + 2] <= 1.0 + epsilon);
		}
	}
	
	
	for (size_t i = 0; i < shapes.size(); i++) {
		for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
			curSq = powf(shapes[i].mesh.positions[3 * v + 0], 2.0f) + powf(shapes[i].mesh.positions[3 * v + 1], 2.0f) + powf(shapes[i].mesh.positions[3 * v + 2], 2.0f);
			if (curSq > maxSq) {
				maxSq = curSq;
			}
		}
	}
	boundRadius = sqrt(maxSq);
	printf("Set bounding sphere to %f for mesh %s\n", boundRadius, filePath);

}

void ObjectRenderer::loadObjShapes(const std::string &objFile)
{
	std::string err = tinyobj::LoadObj(objectShapes, objectMaterials, objFile.c_str());
	if (!err.empty()) {
		std::cerr << err << std::endl;
	}
}

float ObjectRenderer::getRadius() {
	return boundRadius;
}

void ObjectRenderer::setMaterialHandles(GLint matAmbHandle, GLint matDifHandle, GLint matSpecHandle, GLint matShineHandle){
	h_uMatAmbHandle = matAmbHandle;
	h_uMatDifHandle = matDifHandle;
	h_uMatSpecHandle = matSpecHandle;
	h_uMatShineHandle = matShineHandle;
}



/* helper function to send materials to the shader - you must create your own */
void ObjectRenderer::setMaterial(int i) {
//	glUseProgram(ShadeProg);
	switch (i) {
	case 0: //shiny blue plastic
		glUniform3f(h_uMatAmbHandle, 0.02, 0.02, 0.1);
		glUniform3f(h_uMatDifHandle, 0.0, 0.08, 0.5);
		glUniform3f(h_uMatSpecHandle, 0.14, 0.14, 0.4);
		glUniform1f(h_uMatShineHandle, 120.0);
		break;
	case 1: // flat grey
		glUniform3f(h_uMatAmbHandle, 0.13, 0.13, 0.14);
		glUniform3f(h_uMatDifHandle, 0.3, 0.3, 0.4);
		glUniform3f(h_uMatSpecHandle, 0.3, 0.3, 0.4);
		glUniform1f(h_uMatShineHandle, 4.0);
		break;
	case 2: //gold
		glUniform3f(h_uMatAmbHandle, 0.09, 0.07, 0.08);
		glUniform3f(h_uMatDifHandle, 0.91, 0.782, 0.82);
		glUniform3f(h_uMatSpecHandle, 1.0, 0.913, 0.8);
		glUniform1f(h_uMatShineHandle, 200.0);
		break;
	case 3: //something
		glUniform3f(h_uMatAmbHandle, 0.09, 0.07, 0.08);
		glUniform3f(h_uMatDifHandle, 0.31, 0.982, 0.12);
		glUniform3f(h_uMatSpecHandle, 1.0, 0.613, 0.9);
		glUniform1f(h_uMatShineHandle, 20.0);
		break;
	case 4: //something
		glUniform3f(h_uMatAmbHandle, 0.09, 0.07, 0.08);
		glUniform3f(h_uMatDifHandle, 0.41, 0.782, 0.82);
		glUniform3f(h_uMatSpecHandle, 1.0, 0.913, 0.0);
		glUniform1f(h_uMatShineHandle, 100.0);
		break;
	}
}
