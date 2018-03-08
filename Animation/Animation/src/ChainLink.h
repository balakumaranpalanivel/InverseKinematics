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

// chain link - chain
class CChainLink
{
public:
	glm::vec3 mStartPosition;
	glm::vec3 mEndPosition;
	glm::quat mOrientation;
	float mMagnitude;

	// The constraint cone, symbolized by the degrees going in the up, down, left, right directions
	glm::vec4 mConstraintCone;
	void SetConstraintConeDegrees(glm::vec4 degrees);

	// Functions
	CChainLink(glm::vec3 base, glm::vec3 end, float magnitude, glm::quat dir);
	void Render(glm::mat4 view, glm::mat4 proj);
	void Set(glm::vec3 base, glm::vec3 end, float magnitude, glm::quat dir);

	glm::mat4 GetFaceNormals();
	glm::vec3 GetConstraintConeAxis();

private:

	/* Data */
	GLchar * mVertexShader = "C:\\Workspace\\RealtimeAnimation\\InverseKinematics\\OpenGL_Kinematics\\Animation\\Animation\\src\\shaders\\shader.vs";
	GLchar* mFragmentShader = "C:\\Workspace\\RealtimeAnimation\\InverseKinematics\\OpenGL_Kinematics\\Animation\\Animation\\src\\shaders\\shader.frag";
	Shader objectShader;

};

