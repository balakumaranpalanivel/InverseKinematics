#include "ChainLink.h"

#include "EndEffector.h"
#include "Camera.h"

CChainLink::CChainLink(glm::vec3 base, glm::vec3 end, float magnitude, glm::quat dir)
{
	objectShader.LoadShader(mVertexShader, mFragmentShader);
	Set(base, end, magnitude, dir);
}

void CChainLink::Set(glm::vec3 base, glm::vec3 end, float magnitude, glm::quat dir)
{
	mOrientation = dir;
	mStartPosition = base;
	mEndPosition = end;
	this->mMagnitude = magnitude;
	this->mConstraintCone = glm::vec4(45.0f, 45.0f, 45.0f, 45.0f);
}

void CChainLink::Render(glm::mat4 view, glm::mat4 proj)
{
	objectShader.Use();

	GLint objColor = glGetUniformLocation(objectShader.Program, "objectColor");
	GLint lightColor = glGetUniformLocation(objectShader.Program, "lightColor");
	glUniform3f(objColor, 1.0f, 1.0f, 1.0f);
	glUniform3f(lightColor, 1.0f, 1.0f, 1.0f);

	GLint lightPos = glGetUniformLocation(objectShader.Program, "lightPos");
	GLint viewPosLoc = glGetUniformLocation(objectShader.Program, "viewPos");
	glUniform3f(lightPos, 1.0f, 1.0f, 1.0f);
	glUniform3f(viewPosLoc, 0.0, 0.0, 3.0);

	// Calculate the toWorld matrix for the model
	glm::mat4 model;
	glm::mat4 T = glm::translate(glm::mat4(1.0f), mStartPosition);
	glm::mat4 PT = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -0.5));
	glm::mat4 PS = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 1.0f));
	glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, mMagnitude));
	glm::mat4 R = glm::toMat4(mOrientation);

	model = T * R * S * PT * PS;

	glUniformMatrix4fv(glGetUniformLocation(objectShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(objectShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(objectShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(proj));

	// Also set each mesh's shininess property to a default value (if you want you could extend this to another mesh property and possibly change this value)
	glUniform1f(glGetUniformLocation(objectShader.Program, "material.shininess"), 16.0f);

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glDrawArrays(GL_TRIANGLES, 0, 36);

}

glm::mat4 CChainLink::GetFaceNormals()
{

	glm::vec3 up_relative = glm::cross(glm::vec3(0, 0, -1),
		glm::normalize(mEndPosition - mStartPosition));
	glm::vec3 right_relative = glm::cross(glm::vec3(0, 0, -1), up_relative);
	glm::vec3 left_relative = -1.0f * right_relative;
	glm::vec3 down_relative = -1.0f * up_relative;

	return glm::mat4(
		glm::vec4(glm::normalize(up_relative), 0.0f),
		glm::vec4(glm::normalize(down_relative), 0.0f),
		glm::vec4(glm::normalize(left_relative), 0.0f),
		glm::vec4(glm::normalize(right_relative), 0.0f)
	);

}

glm::vec3 CChainLink::GetConstraintConeAxis()
{
	return mEndPosition - mStartPosition;
}

void CChainLink::SetConstraintConeDegrees(glm::vec4 degrees)
{
	mConstraintCone = degrees;
}
