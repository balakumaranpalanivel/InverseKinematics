#pragma once
#include "Shader.h"
#include "Model.h"
#include "Camera.h"

// Std includes
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

// GLM includes
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <GL/glew.h>

class CEndEffector {
public:
	glm::vec3 position;
	float pitch;
	float yaw;
	glm::vec3 scale;

	// Functions
	CEndEffector(int x, int y, int z);
	void Render(glm::mat4 view, glm::mat4 proj);
	void ProcessInput(Camera_Movement direction, GLfloat deltaTime);
	void Animate(std::vector<glm::vec3> interpolated_points);

private:

	/* Data */
	GLchar* pathToModel = "C:\\Workspace\\RealtimeAnimation\\InverseKinematics\\OpenGL_Kinematics\\Animation\\Animation\\src\\shaders\\sphere.off";
	GLchar* vertexShaderPath = "C:\\Workspace\\RealtimeAnimation\\InverseKinematics\\OpenGL_Kinematics\\Animation\\Animation\\src\\shaders\\shader.vs";
	GLchar* fragShaderPath = "C:\\Workspace\\RealtimeAnimation\\InverseKinematics\\OpenGL_Kinematics\\Animation\\Animation\\src\\shaders\\shader.frag";
	
	Model mModel;
	Shader mShader;

	/* Functions */


};
