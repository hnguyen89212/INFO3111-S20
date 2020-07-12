#include "cMeshObject.h"

cMeshObject::cMeshObject() {
	// Clear the glm numbers.
	this->position = glm::vec3(0.0f, 0.0f, 0.0f);
	this->colorRGBA = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	this->orientation = glm::vec3(0.0f, 0.0f, 0.0f);
	this->scale = 1.0;
	this->isWireFrame = false;
}