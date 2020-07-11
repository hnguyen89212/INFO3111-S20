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

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>>
#include <string>

#include "cShaderManager.h"
#include "cVAOManager.h"

// ----------------------------------------------
// Camera

//glm::vec3 g_cameraEye = glm::vec3(0.0f, 60.0f, -100.0f);
//glm::vec3 g_cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
//glm::vec3 g_upVector = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 g_cameraEye = glm::vec3(400.0f, 360.0f, -300.0f);
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
* Q = up
* E = down
*/
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	const float CAMERA_SPEED = 1.f;

	if (key == GLFW_KEY_A)
		::g_cameraEye.x -= CAMERA_SPEED;

	if (key == GLFW_KEY_D)
		::g_cameraEye.x += CAMERA_SPEED;

	if (key == GLFW_KEY_S)
		::g_cameraEye.z -= CAMERA_SPEED;

	if (key == GLFW_KEY_W)
		::g_cameraEye.z += CAMERA_SPEED;

	if (key == GLFW_KEY_E)
		::g_cameraEye.y -= CAMERA_SPEED;

	if (key == GLFW_KEY_Q)
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

	sModelDrawInfo bunnyModel;
	if (!::g_pVAOManager->LoadModelIntoVAO("assets/models/dolphin_color.ply", bunnyModel, program))
		std::cout << "Error: " << ::g_pVAOManager->getLastError() << std::endl;

	while (!glfwWindowShouldClose(window))
	{
		float ratio;
		int width, height;
		glm::mat4 m, p, v, mvp;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);

		m = glm::mat4(1.0f);

		p = glm::perspective(0.6f,
			ratio,
			0.1f,
			1000.0f);

		v = glm::mat4(1.0f);

		v = glm::lookAt(::g_cameraEye,
			::g_cameraTarget,
			::g_upVector);

		mvp = p * v * m;


		glUseProgram(program);

		if (::g_isWireFrame)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

		sModelDrawInfo mdoModelToDraw;
		if (::g_pVAOManager->FindDrawInfoByModelName("assets/models/dolphin_color.ply",
			mdoModelToDraw))
		{
			glBindVertexArray(mdoModelToDraw.VAO_ID);

			glDrawElements(GL_TRIANGLES,
				mdoModelToDraw.numberOfIndices,
				GL_UNSIGNED_INT,     // How big the index values are 
				0);        // Starting index for this model

			glBindVertexArray(0);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete ::g_pShaderManager;

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
