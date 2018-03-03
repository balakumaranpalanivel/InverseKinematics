#include "Target.h"
#include "Camera.h"
#include <GLFW/glfw3.h>

CTarget::CTarget(int x, int y, int z)
{
	CShader modelShader(vertexShaderPath, fragShaderPath);
	mObjectShader = modelShader;

	// Create the model
	CModel model(pathToModel);
	mObjectModel = model;

	/*
		Adding a bit of noise to the target, because if the target 
		starts in a perfect location, the joint might overlap and
		create an issue with the algorithm
	*/

	mPosition = glm::vec3(x, y, z) + 0.0001f;
	mScale = glm::vec3(0.05f, 0.05f, 0.05f);
	mPitch = 0.0f;
	mYaw = 0.0f;
}

void CTarget::Render(glm::mat4 view, glm::mat4 proj)
{
	mObjectShader.Use();

	GLint objectColorLoc = glGetUniformLocation(
		mObjectShader.mProgram, "objectColor"
	);

	GLint lightColorLoc = glGetUniformLocation(
		mObjectShader.mProgram, "lightColor"
	);

	GLint lightPosLoc = glGetUniformLocation(
		mObjectShader.mProgram, "lightPos"
	);

	GLint viewPosLoc = glGetUniformLocation(
		mObjectShader.mProgram, "viewPos"
	);

	glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(lightColorLoc, 1.0f, 0.0f, 0.0f);
	glUniform3f(lightPosLoc, 1.0f, 1.0f, 3.0f);
	glUniform3f(viewPosLoc, 0.0f, 0.0f, 3.0f);

	// Calculate the world matrix for model
	glm::mat4 model;
	glm::mat4 T = glm::translate(glm::mat4(1.0f), mPosition);
	glm::mat4 S = glm::scale(glm::mat4(1.0f), mScale);
	glm::mat4 R = glm::rotate(glm::mat4(1.0f), mPitch, glm::vec3(1, 0, 0));
	R = glm::rotate(R, mYaw, glm::vec3(0, 0, 1));
	model = T * R * S;

	glUniformMatrix4fv(glGetUniformLocation(mObjectShader.mProgram, "model"),
		1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(mObjectShader.mProgram, "view"),
		1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(mObjectShader.mProgram, "projection"),
		1, GL_FALSE, glm::value_ptr(proj));

	mObjectModel.Draw(mObjectShader);
}

void CTarget::ProcessTranslation(Camera_Movement direction, GLfloat deltaTime) {
	GLfloat velocity = 2.0f * deltaTime;
	if (direction == UP)
		this->mPosition.y += 1.0f * velocity;
	if (direction == DOWN)
		this->mPosition.y -= 1.0f * velocity;
	if (direction == LEFT)
		this->mPosition.x -= 1.0f * velocity;
	if (direction == RIGHT)
		this->mPosition.x += 1.0f * velocity;
	if (direction == FORWARD)
		this->mPosition.z += 1.0f * velocity;
	if (direction == BACKWARD)
		this->mPosition.z -= 1.0f * velocity;

#ifdef _DEBUG
	std::cout << "Updated Target Position: (" << this->mPosition.x << "," <<
		this->mPosition.y << "," << this->mPosition.z << ")" << std::endl;
#endif
}
