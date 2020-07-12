#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class cMeshObject {
public:
	cMeshObject();
	~cMeshObject() {};
	std::string meshName;
	glm::vec3 position;
	glm::vec4 colorRGBA;
	glm::vec3 orientation; //rotation:XYZ
	float scale;
	bool isWireFrame;
};