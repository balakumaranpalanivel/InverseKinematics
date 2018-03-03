#include "Segment.h"
#include "Target.h"
#include "Camera.h"

CSegment::CSegment(
	glm::vec3 base,
	glm::vec3 end,
	float magnitude,
	glm::quat dir)
{
	// Initialise Shader
	CShader modelS(vertexShaderPath, fragShaderPath);
	mObjectShader = modelS;

	Set(base, end, magnitude, dir);
}

void CSegment::Set(glm::vec3 base, glm::vec3 end,
	float magnitude, glm::quat dir)
{
	mQuat = dir;
	mPosition = base;
	mEndPosition = end;
	this->mMagnitude = magnitude;
	this->mConstraintCone = glm::vec4(45.0f, 45.0f, 45.0f, 45.0f);
}

void CSegment::Render(glm::mat4 view, glm::mat4 proj)
{
	mObjectShader.Use();

	GLint objectColorLoc = glGetUniformLocation(
		mObjectShader.mProgram, "objectColor");
	GLint lightColorLoc = glGetUniformLocation(
		mObjectShader.mProgram, "lightColor");
	GLint lightPosLoc = glGetUniformLocation(
		mObjectShader.mProgram, "lightPos");
	GLint viewPosLoc = glGetUniformLocation(
		mObjectShader.mProgram, "viewPos");

	glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(lightPosLoc, 1.0f, 1.0f, 3.0f);
	glUniform3f(viewPosLoc, 0.0, 0.0, 3.0);

	// Calculate the toWorld matrix for the model
	glm::mat4 model;
	glm::mat4 T = glm::translate(glm::mat4(1.0f), mPosition);
	glm::mat4 PT = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -0.5));
	glm::mat4 PS = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 1.0f));
	glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, mMagnitude));
	glm::mat4 R = glm::toMat4(mQuat);
	model = T * R * S * PT * PS;

	glUniformMatrix4fv(glGetUniformLocation(mObjectShader.mProgram, "model"),
		1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(mObjectShader.mProgram, "view"),
		1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(mObjectShader.mProgram, "projection"),
		1, GL_FALSE, glm::value_ptr(proj));

	/*
		Setting each mesh's shininess property to a default value
	*/
	glUniform1f(
		glGetUniformLocation(mObjectShader.mProgram, "material.shininess"),
		16.0f
	);

	// Vertex Buffer Content
	// VBO stuff

	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	// first, configure the cube's VAO (and VBO)
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	// position attribute
	glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0
	);
	glEnableVertexAttribArray(0);

	// normal attribute
	glVertexAttribPointer(
		1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))
	);
	glEnableVertexAttribArray(1);

	glDrawArrays(GL_TRIANGLES, 0, 36);
}

glm::mat4 CSegment::GetFaceNormals()
{
	glm::vec3 upRelative = glm::cross(
		glm::vec3(0, 0, -1),
		glm::normalize(mEndPosition - mPosition)
	);

	glm::vec3 rightRelative = glm::cross(
		glm::vec3(0, 0, -1),
		upRelative
	);

	glm::vec3 leftRelative = -1.0f * rightRelative;
	glm::vec3 downRelative = -1.0f * upRelative;

	return glm::mat4(
		glm::vec4(glm::normalize(upRelative), 0.0f),
		glm::vec4(glm::normalize(downRelative), 0.0f),
		glm::vec4(glm::normalize(leftRelative), 0.0f),
		glm::vec4(glm::normalize(rightRelative), 0.0f)
	);
}

glm::vec3 CSegment::GetConstraintConeAxis()
{
	// the axis along the segment itself
	return mEndPosition - mPosition;
}

void CSegment::SetConstraintConeDegrees(glm::vec4 degrees)
{
	mConstraintCone = degrees;
}