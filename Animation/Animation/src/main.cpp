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
#include "MultiChain.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Windows.h>

#include "Spline.h"

// Properties
GLuint screenWidth = 1200, screenHeight = 800;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void Do_Movement(CEndEffector * target);
//void ProcessFrame(const Leap::Controller & controller, CEndEffector * target);

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

// The MAIN function, from here we start our application and run our Game loop
int main()
{
	determine_control_points();

	char desired_model[] = "1";
	//cout << "1: Single chain\n2: Multichain\n3: Single Chain w/ Constraint\nEnter the model you want here: ";
	//cin >> desired_model;
	//cout << desired_model << endl;

	// Init GLFW
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
	if (glewInit() != GLEW_OK) {
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

	// Load our model object
	CEndEffector target(1.0f, 1.0f, 0);
	//CEndEffector target2(2, 0, 0);
	//CEndEffector target3(1, 1, 0);

	// Load joints
	vector<glm::vec3> joints1;
	for (int i = 0; i < 5; ++i)
	{
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		joints1.push_back(glm::vec3(0, r, 0));
	}
	Chain chain1(joints1, &target);

	vector<glm::vec3> joints2;
	joints2.push_back(glm::vec3(0, 0.0f, 0));
	joints2.push_back(glm::vec3(0, 1.0f, 0));
	joints2.push_back(glm::vec3(0.0f, 2.0f, 0));
	//joints2.push_back(glm::vec3(0.0f, 3.0f, 0));

	Chain chain2(joints2, &target);
	vector<glm::vec4> constarins;
	constarins.push_back(glm::vec4(45.0f, 45.0f, 45.0f, 45.0f));
	constarins.push_back(glm::vec4(35.0f, 55.0f, 40.0f, 44.0f));

	chain2.SetConstraint(constarins);
	chain2.please_constraint = true;

	// Load our model object
	CEndEffector target1(0, 1, 0);
	//CEndEffector target2(0, 1, 0);
	//target = target1;

	vector<Chain*> vec;

	CEndEffector target2(1, 0, 0);
	Chain *shoulder = new Chain(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), &target2, 1);
	vec.push_back(shoulder);

	CEndEffector target3(0.5, -1, 0);
	Chain *bodyRight = new Chain(glm::vec3(1, 0, 0), glm::vec3(0.5, -1, 0), &target3, 1);
	vec.push_back(bodyRight);

	CEndEffector target4(0, 0, 0);
	Chain *bodyLeft = new Chain(glm::vec3(0.5, -1, 0), glm::vec3(0, 0, 0), &target4, 1);
	vec.push_back(bodyLeft);

	CEndEffector target5(0, 1, 1);
	Chain *armLeft = new Chain(glm::vec3(0, 0, 0), glm::vec3(0, 1, 1), &target5, 2);
	vec.push_back(armLeft);

	CEndEffector target6(1, 1, 1);
	target = target6;
	Chain *armRight = new Chain(glm::vec3(1, 0, 0), glm::vec3(1, 1, 1), &target, 2);
	vector<glm::vec4> armRightConstraint;
	armRightConstraint.push_back(glm::vec4(0.0f, 0.0f, 0.0f, 90.0f));
	armRightConstraint.push_back(glm::vec4(90.0f, 90.0f, 0.0f, 0.0f));
	armRight->please_constraint = true;
	armRight->SetConstraint(armRightConstraint);
	vec.push_back(armRight);

	//vec.push_back(new Chain(glm::vec3(0, 1, 0), glm::vec3(-1, 1.5, 0), &target2, 1));
	MultiChain multichain(vec);

	// Leap motion stuff
	//Leap::Controller controller;
	bool controller_msg_displayed = false;
	bool isAnimate = true;

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		//if(controller.isConnected() && !controller_msg_displayed) {
		//  cout << "Leap Motion is connected." << endl;
		//  controller_msg_displayed = true;
		//}

		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check and call events
		glfwPollEvents();

		// Clear the colorbuffer
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Do_Movement(&target);
		DoCameraMovement();

		if (points_to_travel.size() > 0 && isAnimate)
		{
			Sleep(40);
			target.mPosition = points_to_travel[0];
			points_to_travel.erase(points_to_travel.begin());
		}

		//if(controller.isConnected()) ProcessFrame(controller, &target);

		// Transformation matrices
		glm::mat4 projection = glm::perspective(camera.GetZoom(), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);

		glm::mat4 view = camera.GetViewMatrix();
		target.Render(view, projection);

		if (strcmp(desired_model, "1") == 0) {
			chain1.Solve();
			chain1.Render(view, projection);
		}
		else if (strcmp(desired_model, "2") == 0) {
			multichain.Solve();
			multichain.Render(view, projection);
			target2.Render(view, projection);
			target3.Render(view, projection);
		}
		else if (strcmp(desired_model, "3") == 0) {
			chain2.Solve();
			chain2.Render(view, projection);
		}
		else {
			cout << "Invalid chain model" << endl;
			break;
		}

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

#pragma region "User input"

// Moves/alters the target position based on user input
void Do_Movement(CEndEffector * target)
{
	if (keys[GLFW_KEY_LEFT_SHIFT] && keys[GLFW_KEY_UP])
		target->ProcessInput(FORWARD, deltaTime);
	else if (keys[GLFW_KEY_UP])
		target->ProcessInput(UP, deltaTime);

	if (keys[GLFW_KEY_LEFT_SHIFT] && keys[GLFW_KEY_DOWN])
		target->ProcessInput(BACKWARD, deltaTime);
	else if (keys[GLFW_KEY_DOWN])
		target->ProcessInput(DOWN, deltaTime);

	if (keys[GLFW_KEY_LEFT])
		target->ProcessInput(LEFT, deltaTime);
	if (keys[GLFW_KEY_RIGHT])
		target->ProcessInput(RIGHT, deltaTime);
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

