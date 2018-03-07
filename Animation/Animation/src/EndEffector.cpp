//
//  CEndEffector.cpp
//

#include "EndEffector.h"
#include "Camera.h"
#include <GLFW/glfw3.h>
#include <Windows.h>

CEndEffector::CEndEffector(int x, int y, int z)
{

	mShader.LoadShader(mVertexShader, mFragmentShader);
	mModel.LoadModel(mModelPath);

	/*
		End effector positions is not the same as
		last segment location as it overlaps with the model
		causing the algorihtm to solve for a position inside the model
		and messing up the algorithm
	*/
	mPosition = glm::vec3(x, y, z) + 0.0001f;
	mScale = glm::vec3(0.05f, 0.05f, 0.05f);
	mPitch = 0.0f;
	mYaw = 0.0f;
}

void CEndEffector::Render(glm::mat4 view, glm::mat4 proj)
{
	mShader.Use();

	GLint objColor = glGetUniformLocation(mShader.Program, "objectColor");
	GLint lightColor = glGetUniformLocation(mShader.Program, "lightColor");
	glUniform3f(objColor, 1.0f, 1.0f, 1.0f);
	glUniform3f(lightColor, 1.0f, 0.0f, 0.0f);

	GLint lightPos = glGetUniformLocation(mShader.Program, "lightPos");
	GLint viewPosLoc = glGetUniformLocation(mShader.Program, "viewPos");
	glUniform3f(lightPos, 1.0f, 1.0f, 3.0f);
	glUniform3f(viewPosLoc, 0.0, 0.0, 3.0);

	glm::mat4 model;
	glm::mat4 T = glm::translate(glm::mat4(1.0f), mPosition);
	glm::mat4 S = glm::scale(glm::mat4(1.0f), mScale);
	glm::mat4 R = glm::eulerAngleX(mPitch) * glm::eulerAngleZ(mYaw);
	model = T * R * S;

	glUniformMatrix4fv(glGetUniformLocation(mShader.Program, "model"), 1,
		GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(mShader.Program, "view"), 1,
		GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(mShader.Program, "projection"), 1,
		GL_FALSE, glm::value_ptr(proj));

	mModel.Draw(mShader);
}

void CEndEffector::ProcessInput(Camera_Movement direction, GLfloat deltaTime)
{
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

	std::cout << "Updated End Effector Position: (" << this->mPosition.x << "," <<
		this->mPosition.y << "," << this->mPosition.z << ")" << std::endl;

}

// Animate the end effector based on the spline interpolated points
void CEndEffector::Animate(std::vector<glm::vec3> interpolated_points)
{
	GLfloat currentFrame = glfwGetTime();
	GLfloat lastFrame = 0.0f;
	GLfloat deltaTime;

	for (std::vector<int>::size_type i = 0; i != interpolated_points.size(); i++)
	{
		//Sleep(300);
		//currentFrame = glfwGetTime();
		//deltaTime = currentFrame - lastFrame;
		//lastFrame = currentFrame;
		this->mPosition = interpolated_points[i];
		std::cout << "Updated End Effector Position: (" << this->mPosition.x << "," <<
			this->mPosition.y << "," << this->mPosition.z << ")" << std::endl;
	}
}
