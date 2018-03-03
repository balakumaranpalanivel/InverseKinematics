#pragma once
#include <stdio.h>
#include <vector>
#include <stack>
#include <map>

#include "Chain.h"

// GLM includes
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/projection.hpp>
#include <GL/glew.h>


struct ChainNode
{
	Chain *value;
	ChainNode *parent;
	vector<ChainNode*>* children;
};

class MultiChain
{
public:
	MultiChain(vector<Chain*> chains);
	bool Insert(ChainNode *root, Chain *chain);
	void Solve();
	void Render(glm::mat4 view, glm::mat4 proj);

	ChainNode *mRoot;
	map<ChainNode*, bool> mLeaves;
	glm::vec3 mOrigin;

private:
	void Forward(ChainNode *root);
	void Backward(ChainNode *root);
};