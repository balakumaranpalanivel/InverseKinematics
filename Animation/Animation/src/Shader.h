#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>

class CShader
{
public:
	GLuint mProgram;

	CShader();

	CShader(const GLchar* vertexPath, const GLchar* fragmentPath);

	void Use();
};