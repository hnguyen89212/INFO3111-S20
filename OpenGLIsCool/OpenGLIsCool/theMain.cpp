/**
Student: Hai Nguyen
ID: 0904995
INFO-3111-(01-02)-20S
Class C
Checkpoint #03
*/

#include "globalOpenGLHeader.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <sstream> 
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>>
#include <string>
#include <vector>

#include "cShaderManager.h"
#include "cVAOManager.h"
#include "cMeshObject.h"

// ----------------------------------------------
// To draw multiple models, removed later
std::vector<cMeshObject*> g_pVecMeshObjects;

// ----------------------------------------------
// Camera

//glm::vec3 g_cameraEye = glm::vec3(0.0f, 60.0f, -100.0f);
//glm::vec3 g_cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
//glm::vec3 g_upVector = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 g_cameraEye = glm::vec3(0.0f, 0.0f, 3000.0f);
glm::vec3 g_cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_upVector = glm::vec3(0.0f, 1.0f, 0.0f);

// ----------------------------------------------
// Managers

cShaderManager* g_pShaderManager = 0;
cVAOManager* g_pVAOManager = 0;

// ----------------------------------------------
// wire frame vs solid
bool g_isWireFrame = false;

// ----------------------------------------------
// Error handler
static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

// ----------------------------------------------
// Key handler
/**
* A = right
* D = left
* S = forward
* W = backward
* E = up
* Q = down
*
* Z = scale selected object up
* X = scale selected object down
* F = jump forward (index++)
* B = jump backward (index--)
*
* P = persist all cMeshObject in list to file.
* L = load all cMeshObject from file "" to ::g_pVecMeshObjects
*/

int selectedIndex = 0;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	const float CAMERA_SPEED = 1.0f;

	if (key == GLFW_KEY_P)
	{
		std::ofstream storage;
		storage.open("storage.txt", std::ios::trunc);
		storage << ::g_pVecMeshObjects.size() << std::endl;
		for (int i = 0; i < ::g_pVecMeshObjects.size(); i++)
		{
			storage << ::g_pVecMeshObjects.at(i)->meshName;
			storage << std::endl;
			// position
			storage << ::g_pVecMeshObjects.at(i)->position.x
				<< "," << ::g_pVecMeshObjects.at(i)->position.y
				<< "," << ::g_pVecMeshObjects.at(i)->position.z
				<< ",";
			storage << std::endl;
			// color
			storage << ::g_pVecMeshObjects.at(i)->colorRGBA.x
				<< "," << ::g_pVecMeshObjects.at(i)->colorRGBA.y
				<< "," << ::g_pVecMeshObjects.at(i)->colorRGBA.z
				<< "," << ::g_pVecMeshObjects.at(i)->colorRGBA.w
				<< ",";
			storage << std::endl;
			// orientation
			storage << ::g_pVecMeshObjects.at(i)->orientation.x
				<< "," << ::g_pVecMeshObjects.at(i)->orientation.y
				<< "," << ::g_pVecMeshObjects.at(i)->orientation.z
				<< ",";
			storage << std::endl;
			// scale
			storage << ::g_pVecMeshObjects.at(i)->scale << std::endl;
			// isWireframe
			storage << (::g_pVecMeshObjects.at(i)->isWireFrame) ? 1 : 0;
			storage << std::endl;
		}
		storage << "#";
		storage.close();
	}

	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		::g_pVecMeshObjects.clear();
		std::ifstream storage;
		storage.open("storage.txt");

		std::string line;
		std::string token;

		std::string delimeter = ",";
		size_t pos = 0;

		storage >> line;
		size_t size = 0;
		std::stringstream(line) >> size;

		int counter = 0;

		for (size_t i = 0; i < size; i++)
		{
			storage >> line;
			if (line.find("assets/models/") != std::string::npos)
			{
				cMeshObject* pMesh = new cMeshObject();
				// read mesh name
				pMesh->meshName = line;
				// read position
				storage >> line;
				while ((pos = line.find(delimeter)) != std::string::npos)
				{
					token = line.substr(0, pos);
					line.erase(0, pos + delimeter.length());
					counter++;
					if (counter == 1) pMesh->position.x = std::stof(token);
					else if (counter == 2) pMesh->position.y = std::stof(token);
					else pMesh->position.z = std::stof(token);
				}
				counter = 0; // reset
				// read rgba
				storage >> line;
				while ((pos = line.find(delimeter)) != std::string::npos)
				{
					token = line.substr(0, pos);
					line.erase(0, pos + delimeter.length());
					counter++;
					if (counter == 1) pMesh->colorRGBA.x = std::stof(token);
					else if (counter == 2) pMesh->colorRGBA.y = std::stof(token);
					else if (counter == 3) pMesh->colorRGBA.z = std::stof(token);
					else pMesh->colorRGBA.w = std::stof(token);
				}
				counter = 0; // reset
				// read orientation
				storage >> line;
				while ((pos = line.find(delimeter)) != std::string::npos)
				{
					token = line.substr(0, pos);
					line.erase(0, pos + delimeter.length());
					counter++;
					if (counter == 1) pMesh->orientation.x = std::stof(token);
					else if (counter == 2) pMesh->orientation.y = std::stof(token);
					else pMesh->orientation.z = std::stof(token);
				}
				counter = 0; // reset
				// read scale
				storage >> line;
				pMesh->scale = std::stof(line);
				// read isWireFrame
				storage >> line;
				pMesh->isWireFrame = (std::stof(line) == 1.0) ? true : false;
				::g_pVecMeshObjects.push_back(pMesh);
			}
		} // end for
		storage.close();		
	}

	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		if (selectedIndex == g_pVecMeshObjects.size() - 1) return;
		selectedIndex++;
	}

	if (key == GLFW_KEY_B && action == GLFW_PRESS)
	{
		if (selectedIndex == 0) return;
		selectedIndex--;
	}

	if (key == GLFW_KEY_Z)
	{
		g_pVecMeshObjects[selectedIndex]->scale *= 2.f;
	}

	if (key == GLFW_KEY_X)
	{
		g_pVecMeshObjects[selectedIndex]->scale *= 0.5f;
	}

	if (key == GLFW_KEY_A)
		::g_cameraEye.x -= CAMERA_SPEED;

	if (key == GLFW_KEY_D)
		::g_cameraEye.x += CAMERA_SPEED;

	if (key == GLFW_KEY_S)
		::g_cameraEye.z -= CAMERA_SPEED;

	if (key == GLFW_KEY_W)
		::g_cameraEye.z += CAMERA_SPEED;

	if (key == GLFW_KEY_Q)
		::g_cameraEye.y -= CAMERA_SPEED;

	if (key == GLFW_KEY_E)
		::g_cameraEye.y += CAMERA_SPEED;

	// Closes the window
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	// Switches from wire frame (9) to solid (0)
	if (key == GLFW_KEY_9 && action == GLFW_PRESS) ::g_isWireFrame = true;
	if (key == GLFW_KEY_0 && action == GLFW_PRESS) ::g_isWireFrame = false;

	return;
}

//static const std::string PATH_TO_MODELS = "./assets/models/";

int main(void)
{
	std::cout << "About to start..." << std::endl;

	GLFWwindow* window;
	GLuint vertex_buffer;
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint program;

	GLint mvp_location, vpos_location, vcol_location;

	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
	{
		std::cout << "Can't init GLFW. Exiting" << std::endl;
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	std::cout << "About to create window..." << std::endl;
	window = glfwCreateWindow(1024, 700, "Press 9|0 to switch to wire frame|solid", NULL, NULL);
	// Fails to create window
	if (!window)
	{
		std::cout << "ERROR: Can't create window." << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	::g_pShaderManager = new cShaderManager();          // HEAP  

	cShaderManager::cShader vertShader;
	cShaderManager::cShader fragShader;

	vertShader.fileName = "simpleVertex.glsl";
	fragShader.fileName = "simpleFragment.glsl";

	// There was problem. 
	if (!::g_pShaderManager->createProgramFromFile("JustFriendlyName", vertShader, fragShader))
	{
		std::cout << "ERROR: can't make shader program because: " << std::endl;
		std::cout << ::g_pShaderManager->getLastError() << std::endl;
		// Exit program (maybe add some cleanup code)
		return -1;
	}

	program = ::g_pShaderManager->getIDFromFriendlyName("JustFriendlyName");

	mvp_location = glGetUniformLocation(program, "MVP");
	vpos_location = glGetAttribLocation(program, "vPos");
	vcol_location = glGetAttribLocation(program, "vCol");

	::g_pVAOManager = new cVAOManager();

	/**
	For the .ply files, convert them into the correct format via exporting mesh in MeshLab.
	We can actually load multiple models into memory. See the cVAOManager for more info.
	Inside cVAOManager, there is a "dictionary", we could pass on the exact name of a model to draw, no need to reload it into memory.
	*/

	sModelDrawInfo mdiBunny;
	if (!::g_pVAOManager->LoadModelIntoVAO("assets/models/bun_zipper_res2_xyz_n_rgba_uv.ply", mdiBunny, program))
	{
		std::cout << "Error: " << ::g_pVAOManager->getLastError() << std::endl;
	}

	/*
	sModelDrawInfo mdiDolphin;
	if (!::g_pVAOManager->LoadModelIntoVAO("assets/models/dolphin_color.ply", mdiDolphin, program))
	{
		std::cout << "Error: " << ::g_pVAOManager->getLastError() << std::endl;
	}

	sModelDrawInfo mdiGourard;
	if (!::g_pVAOManager->LoadModelIntoVAO("assets/models/gourard_xyz_rgba.ply", mdiGourard, program))
	{
		std::cout << "Error: " << ::g_pVAOManager->getLastError() << std::endl;
	}

	sModelDrawInfo mdiCorellianCorvette;
	if (!::g_pVAOManager->LoadModelIntoVAO("assets/models/Corellian_Corvette_(Blockade_Runner)_xyz_rgba.ply", mdiCorellianCorvette, program))
	{
		std::cout << "Error: " << ::g_pVAOManager->getLastError() << std::endl;
	}

	sModelDrawInfo mdiSeaFloor;
	if (!::g_pVAOManager->LoadModelIntoVAO("assets/models/Seafloor2_xyz_rgba.ply", mdiSeaFloor, program))
	{
		std::cout << "Error: " << ::g_pVAOManager->getLastError() << std::endl;
	}

	sModelDrawInfo mdiSu74;
	if (!::g_pVAOManager->LoadModelIntoVAO("assets/models/su47_xyz_rgba.ply", mdiSu74, program))
	{
		std::cout << "Error: " << ::g_pVAOManager->getLastError() << std::endl;
	}

	sModelDrawInfo mdiPacificCod0;
	if (!::g_pVAOManager->LoadModelIntoVAO("assets/models/PacificCod0_xyz_rgba.ply", mdiPacificCod0, program))
	{
		std::cout << "Error: " << ::g_pVAOManager->getLastError() << std::endl;
	}
	*/

	/**
	Add to the list of cMeshObjects to draw.
	*/

	cMeshObject* pBunny = new cMeshObject();
	pBunny->meshName = "assets/models/bun_zipper_res2_xyz_n_rgba_uv.ply";
	pBunny->scale = 1000.f;
	//pBunny->position.y += 200.0f;
	::g_pVecMeshObjects.push_back(pBunny);

	/*
	cMeshObject* pDolphin = new cMeshObject();
	pDolphin->meshName = "assets/models/dolphin_color.ply";
	pDolphin->scale = 0.7f; // half of its original size
	::g_pVecMeshObjects.push_back(pDolphin);

	cMeshObject* pGourard = new cMeshObject();
	pGourard->meshName = "assets/models/gourard_xyz_rgba.ply";
	pGourard->position.x -= 700.0f;
	pGourard->position.y -= 1000.0f;
	pGourard->scale = 0.10f; // half of its original size
	//pGourard->orientation.z = glm::radians(135.0f); // rotate around the z-axis 135 degree
	::g_pVecMeshObjects.push_back(pGourard);

	cMeshObject* pCorellianCorvette = new cMeshObject();
	pCorellianCorvette->meshName = "assets/models/Corellian_Corvette_(Blockade_Runner)_xyz_rgba.ply";
	pCorellianCorvette->scale = 10.f;
	pCorellianCorvette->position.y -= 250.0f;
	::g_pVecMeshObjects.push_back(pCorellianCorvette);

	cMeshObject* pSeaFloor = new cMeshObject();
	pSeaFloor->meshName = "assets/models/Seafloor2_xyz_rgba.ply";
	pSeaFloor->scale = 5.f;
	pSeaFloor->position.y -= 500.0f;
	::g_pVecMeshObjects.push_back(pSeaFloor);

	cMeshObject* pSu74 = new cMeshObject();
	pSu74->meshName = "assets/models/su47_xyz_rgba.ply";
	pSu74->scale = 1500.f;
	pSu74->position.y += 500.0f;
	::g_pVecMeshObjects.push_back(pSu74);

	cMeshObject* pPacificCod0 = new cMeshObject();
	pPacificCod0->meshName = "assets/models/PacificCod0_xyz_rgba.ply";
	pPacificCod0->scale = 1700.f;
	pPacificCod0->position.y += 500.0f;
	pPacificCod0->position.x -= 700.0f;
	::g_pVecMeshObjects.push_back(pPacificCod0);
	*/

	while (!glfwWindowShouldClose(window)) {
		float ratio;
		int width, height;
		// mathModel = model matrix
		// p = projection matrix
		// v = view matrix
		// mvp = model view projection
		glm::mat4 matModel, p, v, mvp;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);

		/*matModel = glm::mat4(1.0f);*/

		// view frustum
		p = glm::perspective(0.6f,
			ratio,
			0.1f,			// near plane, anything nearer than this won't be drawn.
			50000.0f); // far plane, anything further than this won't be drawn.

		v = glm::mat4(1.0f);

		v = glm::lookAt(::g_cameraEye,
			::g_cameraTarget,
			::g_upVector);

		/**
		Draw all the mesh objects in vector right here.
		*/
		for (
			std::vector<cMeshObject*>::iterator it_pMesh = ::g_pVecMeshObjects.begin();
			it_pMesh != ::g_pVecMeshObjects.end();
			it_pMesh++
			)
		{
			cMeshObject* pCurrentMesh = *it_pMesh;

			// model matrix represents where object is placed in space.
			matModel = glm::mat4(1.0f);

			/**
			Model transformations
			*/
			// Place the object in the world at 'this' location
			glm::mat4 matTranslation
				= glm::translate(
					glm::mat4(1.0f),
					glm::vec3(pCurrentMesh->position.x, pCurrentMesh->position.y, pCurrentMesh->position.z)
				);
			matModel = matModel * matTranslation;


			//mat4x4_rotate_Z(m, m, );
			//*************************************
			// ROTATE around Z
			glm::mat4 matRotateZ = glm::rotate(glm::mat4(1.0f),
				pCurrentMesh->orientation.z, // (float) glfwGetTime(), 
				glm::vec3(0.0f, 0.0f, 1.0f));
			matModel = matModel * matRotateZ;
			//*************************************

			//*************************************
			// ROTATE around Y
			glm::mat4 matRotateY = glm::rotate(glm::mat4(1.0f),
				pCurrentMesh->orientation.y, // (float) glfwGetTime(), 
				glm::vec3(0.0f, 1.0f, 0.0f));
			matModel = matModel * matRotateY;
			//*************************************

			//*************************************
			// ROTATE around X
			glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
				pCurrentMesh->orientation.x, // (float) glfwGetTime(), 
				glm::vec3(1.0f, 0.0, 0.0f));
			matModel = matModel * rotateX;
			//*************************************


			// Set up a scaling matrix
			glm::mat4 matScale = glm::mat4(1.0f);

			float theScale = pCurrentMesh->scale;		// 1.0f;		
			matScale = glm::scale(glm::mat4(1.0f),
				glm::vec3(theScale, theScale, theScale));


			// Apply (multiply) the scaling matrix to 
			// the existing "model" (or "world") matrix
			matModel = matModel * matScale;

			mvp = p * v * matModel;

			glUseProgram(program);

			if (pCurrentMesh->isWireFrame) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // solid
			}

			glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

			/**
			Look inside the "dictionary" for the model to draw.
			*/

			sModelDrawInfo mdoModelToDraw;
			if (::g_pVAOManager->FindDrawInfoByModelName(pCurrentMesh->meshName, mdoModelToDraw)) {
				glBindVertexArray(mdoModelToDraw.VAO_ID);

				glDrawElements(GL_TRIANGLES,
					mdoModelToDraw.numberOfIndices,
					GL_UNSIGNED_INT,     // How big the index values are 
					0);        // Starting index for this model

				glBindVertexArray(0);
			}
		} // end for loop

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete ::g_pShaderManager;

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
