// Std. Includes
#include <string>
#include <iostream>
#include <cstring>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "EndEffector.h"
#include "Chain.h"
#include "Humanoid.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Windows.h>

#include "Spline.h"

#include "Config.h"

// Properties
GLuint screenWidth = 1200, screenHeight = 800;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void Do_Movement(CEndEffector * target);

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset, true);
}

void DoCameraMovement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}

	if (keys[GLFW_KEY_S])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}

	if (keys[GLFW_KEY_A])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}

	if (keys[GLFW_KEY_D])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}

int main()
{
	// Animate only if a valid Spline configuration is provided
	if (CONFIG.SPLINE_CONFIG >= 0)
	{
		determine_control_points();
	}

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Inverse Kinematics", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, MouseCallback);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		return 0;
	}
	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	/*
		Randomised Simple Chain
	*/
	// Animated End effector
	CEndEffector animatedEnd(1.0f, 1.0f, 0);
	// Load joints
	vector<glm::vec3> joints1;
	for (int i = 0; i < 4; ++i)
	{
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		joints1.push_back(glm::vec3(0, r, 0));
	}
	Chain simpleChain(joints1, &animatedEnd);


	/*
		Primitive humanoid structure 
		built using simple chains
	*/
	// Load our model object
	CEndEffector end1(0, 1, 0);

	// collection of bones in the structure
	vector<Chain*> bones;

	CEndEffector end2(1, 0, 0);
	Chain *shoulder = new Chain(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), &end2, 1);
	bones.push_back(shoulder);

	CEndEffector end3(0.5, -1, 0);
	Chain *bodyRight = new Chain(glm::vec3(1, 0, 0), glm::vec3(0.5, -1, 0), &end3, 1);
	bones.push_back(bodyRight);

	CEndEffector end4(0, 0, 0);
	Chain *bodyLeft = new Chain(glm::vec3(0.5, -1, 0), glm::vec3(0, 0, 0), &end4, 1);
	bones.push_back(bodyLeft);

	CEndEffector end5(0, 1, 1);
	Chain *armLeft = new Chain(glm::vec3(0, 0, 0), glm::vec3(0, 1, 1), &end5, 2);
	bones.push_back(armLeft);

	CEndEffector end6(1, 1, 1);
	animatedEnd = end6;
	Chain *armRight = new Chain(glm::vec3(1, 0, 0), glm::vec3(1, 1, 1), &animatedEnd, 2);
	// List of constraint for the two limbs in the right arm
	vector<glm::vec4> armRightConstraint;
	armRightConstraint.push_back(glm::vec4(0.0f, 0.0f, 0.0f, 90.0f));
	armRightConstraint.push_back(glm::vec4(90.0f, 90.0f, 0.0f, 0.0f));

	armRight->SetConstraint(armRightConstraint);
	bones.push_back(armRight);
	CHumanoid humanoid(bones);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check and call events
		glfwPollEvents();

		// Clear the colorbuffer
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Process Target Movement based on User Input
		Do_Movement(&animatedEnd);

		// Process Camera Movement based on User Input
		DoCameraMovement();

		/*
			Primitive Animation Seqeunce
		*/
		if (points_to_travel.size() > 0)
		{
			Sleep(40);
			animatedEnd.mPosition = points_to_travel[0];
			points_to_travel.erase(points_to_travel.begin());
		}

		// Transformation matrices
		glm::mat4 projection = glm::perspective(camera.GetZoom(), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		animatedEnd.Render(view, projection);

		switch (CONFIG.CHAIN_CONFIG)
		{
			case 0:
			{
				simpleChain.Solve();
				simpleChain.Render(view, projection);
				break;
			}
			case 1:
			{
				humanoid.Solve();
				humanoid.Render(view, projection);
				break;
			}
			default:
			{
				cout << "Invalid Chain Config" << endl;
				break;
			}
		}

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

#pragma region "User input"

// Moves/alters the target position based on user input
void Do_Movement(CEndEffector * pPoint)
{
	if (keys[GLFW_KEY_LEFT_SHIFT] && keys[GLFW_KEY_UP])
		pPoint->ProcessInput(FORWARD, deltaTime);
	else if (keys[GLFW_KEY_UP])
		pPoint->ProcessInput(UP, deltaTime);

	if (keys[GLFW_KEY_LEFT_SHIFT] && keys[GLFW_KEY_DOWN])
		pPoint->ProcessInput(BACKWARD, deltaTime);
	else if (keys[GLFW_KEY_DOWN])
		pPoint->ProcessInput(DOWN, deltaTime);

	if (keys[GLFW_KEY_LEFT])
		pPoint->ProcessInput(LEFT, deltaTime);
	if (keys[GLFW_KEY_RIGHT])
		pPoint->ProcessInput(RIGHT, deltaTime);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		fprintf(stderr, "Click\n");
	}
}

#pragma endregion

