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
#include <glm/gtx/euler_angles.hpp>
#include <GL/glew.h>

class CEndEffector
{
public:
	glm::vec3 mPosition;
	float mPitch, mYaw;
	glm::vec3 mScale;

	// Functions
	CEndEffector(int x, int y, int z);

	void Render(glm::mat4 view, glm::mat4 proj);
	void Animate(std::vector<glm::vec3> interpolated_points);

	void ProcessInput(Camera_Movement direction, GLfloat deltaTime);

private:

	/* Data */
	GLchar* mModelPath = "C:\\Workspace\\RealtimeAnimation\\InverseKinematics\\OpenGL_Kinematics\\Animation\\Animation\\src\\shaders\\sphere.off";
	GLchar* mVertexShader = "C:\\Workspace\\RealtimeAnimation\\InverseKinematics\\OpenGL_Kinematics\\Animation\\Animation\\src\\shaders\\shader.vs";
	GLchar* mFragmentShader = "C:\\Workspace\\RealtimeAnimation\\InverseKinematics\\OpenGL_Kinematics\\Animation\\Animation\\src\\shaders\\shader.frag";
	
	Model mModel;
	Shader mShader;


};
