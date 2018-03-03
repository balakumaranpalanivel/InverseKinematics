#pragma once
#pragma once

// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

// GL Includes
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

GLint TextureFromFile(const char* path, string directory);

class CModel
{
public:
	/*  Functions   */
	// Constructor, expects a filepath to a 3D model.
	CModel() {}
	CModel(GLchar* path);

	// Draws the model, and thus all its meshes
	void Draw(CShader shader);

private:
	/*  Model Data  */
	vector<CMesh> meshes;
	string directory;
	/*
		Stores all the textures loaded so far, optimization to
		make sure textures aren't loaded more than once.
	*/
	vector<Texture> textures_loaded;	

	// Loads a model and stores the meshes in the meshes vector.
	void LoadModel(string path);

	/*
		Processes a node in a recursive fashion. Processes each individual 
		mesh located at the node and repeats this process on its 
		children nodes (if any).
	*/
	void ProcessNode(aiNode* node, const aiScene* scene);

	CMesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
};

