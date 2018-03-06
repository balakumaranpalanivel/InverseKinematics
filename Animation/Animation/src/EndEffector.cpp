//
//  CEndEffector.cpp
//

#include "EndEffector.h"
#include "Camera.h"
#include <GLFW/glfw3.h>
#include <Windows.h>

CEndEffector::CEndEffector(int x, int y, int z) {

	// Create the shader to use for the controller
	Shader modelS(vertexShaderPath, fragShaderPath);
	objectShader = modelS;

	// Creates the model for the controller
	Model modelM(pathToModel);
	objectModel = modelM;

	// Add a bit of noise to the target, because if the target
	// starts in a perfect location, the joints might overlap which
	// messes up the algorithm
	position = glm::vec3(x, y, z) + 0.0001f;
	scale = glm::vec3(.05f, .05f, .05f);
	pitch = 0.0f;
	yaw = 0.0f;
}

void CEndEffector::Render(glm::mat4 view, glm::mat4 proj) {

	objectShader.Use();

	GLint objectColorLoc = glGetUniformLocation(objectShader.Program, "objectColor");
	GLint lightColorLoc = glGetUniformLocation(objectShader.Program, "lightColor");
	GLint lightPosLoc = glGetUniformLocation(objectShader.Program, "lightPos");
	GLint viewPosLoc = glGetUniformLocation(objectShader.Program, "viewPos");

	glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(lightColorLoc, 1.0f, 0.0f, 0.0f);
	glUniform3f(lightPosLoc, 1.0f, 1.0f, 3.0f);
	glUniform3f(viewPosLoc, 0.0, 0.0, 3.0);

	// Calculate the toWorld matrix for the model
	glm::mat4 model;
	glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
	glm::mat4 R = glm::rotate(glm::mat4(1.0f), pitch, glm::vec3(1, 0, 0));
	R = glm::rotate(R, yaw, glm::vec3(0, 0, 1));
	model = T * R * S;

	glUniformMatrix4fv(glGetUniformLocation(objectShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(objectShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(objectShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(proj));

	objectModel.Draw(objectShader);

}

void CEndEffector::ProcessTranslation(Camera_Movement direction, GLfloat deltaTime) {
	GLfloat velocity = 2.0f * deltaTime;
	if (direction == UP)
		this->position.y += 1.0f * velocity;
	if (direction == DOWN)
		this->position.y -= 1.0f * velocity;
	if (direction == LEFT)
		this->position.x -= 1.0f * velocity;
	if (direction == RIGHT)
		this->position.x += 1.0f * velocity;
	if (direction == FORWARD)
		this->position.z += 1.0f * velocity;
	if (direction == BACKWARD)
		this->position.z -= 1.0f * velocity;

	std::cout << "Updated CEndEffector Position: (" << this->position.x << "," <<
		this->position.y << "," << this->position.z << ")" << std::endl;

}


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
		this->position = interpolated_points[i];
		std::cout << "Updated CEndEffector Position: (" << this->position.x << "," <<
			this->position.y << "," << this->position.z << ")" << std::endl;

	}

	//GLfloat velocity = 2.0f * deltaTime;
	//if (direction == UP)
	//	this->position.y += 1.0f * velocity;
	//if (direction == DOWN)
	//	this->position.y -= 1.0f * velocity;
	//if (direction == LEFT)
	//	this->position.x -= 1.0f * velocity;
	//if (direction == RIGHT)
	//	this->position.x += 1.0f * velocity;
	//if (direction == FORWARD)
	//	this->position.z += 1.0f * velocity;
	//if (direction == BACKWARD)
	//	this->position.z -= 1.0f * velocity;

	//std::cout << "Updated CEndEffector Position: (" << this->position.x << "," <<
	//	this->position.y << "," << this->position.z << ")" << std::endl;

}
