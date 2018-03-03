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

class CSegment
{
public:
	glm::vec3 mPosition;
	glm::vec3 mEndPosition;
	glm::quat mQuat;
	float mMagnitude;

	// Constrain cone to limit the movement of the segment
	glm::vec4 mConstraintCone;
	// The cone has four degres
	void SetConstraintConeDegrees(glm::vec4 degrees);

	CSegment(glm::vec3 base, glm::vec3 end, float magnitude, glm::quat dir);
	void Render(glm::mat4 view, glm::mat4 proj);
	void ProcessTranslation(Camera_Movement direction, GLfloat deltaTime);
	void Set(glm::vec3 base, glm::vec3 end, float magnitude, glm::quat dir);

	/*
		0, 1, 2, 3 - Up, Down, Left, Right
		Wrapping each index around a vec3
	*/
	glm::mat4 GetFaceNormals();
	glm::vec3 GetConstraintConeAxis();

private:
	CShader mObjectShader;

	GLchar* vertexShaderPath = "C:\\Workspace\\RealtimeAnimation\\InverseKinematics\\OpenGL_Kinematics\\Animation\\Animation\\src\\shaders\\segment.vs";
	GLchar* fragShaderPath = "C:\\Workspace\\RealtimeAnimation\\InverseKinematics\\OpenGL_Kinematics\\Animation\\Animation\\src\\shaders\\segment.frag";

};
