#include "Shader.h"

CShader::CShader()
{

}

CShader::CShader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
	// Read source code from the files
	std::string vertexSource;
	std::string fragmentSource;
	
	// input streams
	std::ifstream vertexShader;
	std::ifstream fragmentShader;

	vertexShader.exceptions(std::ifstream::badbit);
	fragmentShader.exceptions(std::ifstream::badbit);
	try
	{
		// open files
		vertexShader.open(vertexPath);
		fragmentShader.open(fragmentPath);

		std::stringstream vertexShaderStream;
		std::stringstream fragmentShaderStream;

		// Read file's buffer content into stream
		vertexShaderStream << vertexShader.rdbuf();
		fragmentShaderStream << fragmentShader.rdbuf();

		// close the handlers
		vertexShader.close();
		fragmentShader.close();

		// convert to string
		vertexSource = vertexShaderStream.str();
		fragmentSource = fragmentShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	// compile the read source code
	const GLchar* vShaderCode = vertexSource.c_str();
	const GLchar* fShaderCode = fragmentSource.c_str();

	GLuint vertex, fragment;
	GLint success;
	GLchar infoLog[512];

	// Vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);

	// print compile errors
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR:SHADER:VERTEX:COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	// Print compile errors if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Shader Program
	this->mProgram = glCreateProgram();
	glAttachShader(this->mProgram, vertex);
	glAttachShader(this->mProgram, fragment);
	glLinkProgram(this->mProgram);

	// Print Linking Errors
	glGetProgramiv(this->mProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->mProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER:PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// Delete the shaders
	glDeleteShader(vertex);
	glDeleteShader(fragment);

}

void CShader::Use()
{
	glUseProgram(this->mProgram);
}