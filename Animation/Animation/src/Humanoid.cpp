#include "Humanoid.h"

CHumanoid::CHumanoid(vector<Chain*> chains) {

	root = new ChainNode();
	root->value = *chains.begin();
	root->parent = NULL;
	root->children = new vector<ChainNode*>();
	bool success = false;

	origin = root->value->origin;

	for (auto it = chains.begin() + 1; it != chains.end(); ++it) {
		Chain * current = *it;
		success = true && Insert(root, current);
	}

	if (!success) {
		cerr << "Something went wrong when constructing the multichain; one of the nodes probably didn't match up." << endl;
		exit(-1);
	}
}

CHumanoid::CHumanoid() {

	BuildHumanoid();

	root = new ChainNode();
	root->value = *mChainList.begin();
	root->parent = NULL;
	root->children = new vector<ChainNode*>();
	bool success = false;

	origin = root->value->origin;

	for (auto it = mChainList.begin() + 1; it != mChainList.end(); ++it) {
		Chain * current = *it;
		success = true && Insert(root, current);
	}

	if (!success) {
		cerr << "Something went wrong when constructing the multichain; one of the nodes probably didn't match up." << endl;
		exit(-1);
	}
}


void CHumanoid::BuildHumanoid()
{
	vector<Chain*> vec;

	CEndEffector target2(1, 0, 0);
	Chain *shoulder = new Chain(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), &target2, 1);
	mChainList.push_back(shoulder);

	CEndEffector target3(0.5, -1, 0);
	Chain *bodyRight = new Chain(glm::vec3(1, 0, 0), glm::vec3(0.5, -1, 0), &target3, 1);
	mChainList.push_back(bodyRight);

	CEndEffector target4(0, 0, 0);
	Chain *bodyLeft = new Chain(glm::vec3(0.5, -1, 0), glm::vec3(0, 0, 0), &target4, 1);
	mChainList.push_back(bodyLeft);

	CEndEffector target5(0, 1, 1);
	Chain *armLeft = new Chain(glm::vec3(0, 0, 0), glm::vec3(0, 1, 1), &target5, 2);
	mChainList.push_back(armLeft);

	CEndEffector target6(1, 1, 1);
	Chain *armRight = new Chain(glm::vec3(1, 0, 0), glm::vec3(1, 1, 1), &target6, 2);
	vector<glm::vec4> armRightConstraint;
	armRightConstraint.push_back(glm::vec4(0.0f, 0.0f, 0.0f, 90.0f));
	armRightConstraint.push_back(glm::vec4(90.0f, 90.0f, 0.0f, 0.0f));
	armRight->please_constraint = true;
	armRight->SetConstraint(armRightConstraint);
	mChainList.push_back(armRight);
}

bool CHumanoid::Insert(ChainNode * root, Chain * chain) {
	if (glm::all(glm::equal(root->value->end, chain->origin))) {

		ChainNode * new_node = new ChainNode();
		new_node->value = chain;
		new_node->parent = root;
		new_node->children = new vector<ChainNode*>();

		root->children->push_back(new_node);
		root->value->target = new CEndEffector(0, 0, 0);

		// if this node is a leaf, flag it. Otherwise, unflag it
		if (leaves[root]) leaves[root] = false;
		leaves[new_node] = true;

		return true;

	}
	else if (root->children->size()) {

		bool was_inserted = false;
		for (auto it = root->children->begin(); it != root->children->end(); ++it) {
			ChainNode * recurse = *it;
			was_inserted = was_inserted || Insert(recurse, chain);
			if (was_inserted) break;
		}
		return was_inserted;

	}

	return false; // Cannot insert
}

void CHumanoid::Solve() {

	root->value->origin = origin;
	root->value->SetFirstJoint(origin);

	Backward(root);
	root->value->Solve();
	Forward(root);

}

void CHumanoid::Backward(ChainNode * root) {

	if (root->children->size()) {
		for (auto it = root->children->begin(); it != root->children->end(); ++it) {
			Backward(*it);
		}

		// Calculate the centroid
		glm::vec3 centroid;
		int i = 0;
		for (auto it = root->children->begin(); it != root->children->end(); ++it) {
			ChainNode * node = *it;
			centroid += node->value->GetFirstJoint();
			++i;
		}
		centroid = centroid / (float)i;

		// Set the centroid to the target; this should solve toward the centroid now

		//if(glm::length(centroid - root->value->origin) < root->value->total_length)
		root->value->target->mPosition = centroid;
	}

	if (root->parent) {
		root->value->Backward();
	}
}

void CHumanoid::Forward(ChainNode * root) {
	if (root->children->size()) {
		for (auto it = root->children->begin(); it != root->children->end(); ++it) {
			Forward(*it);
		}
	}

	if (root->parent) {
		root->value->origin = root->parent->value->end;
		root->value->Forward();
		root->value->AssignLinks();
	}
}

void CHumanoid::Render(glm::mat4 view, glm::mat4 proj) {

	// Traverse tree and render each chain. Doesn't really matter in what order
	// Not using recursion because don't want to add an unnecessary parameter lol

	stack<ChainNode*> traverse;
	traverse.push(root);

	while (!traverse.empty()) {
		ChainNode * cur = traverse.top();

		cur->value->Render(view, proj);
		traverse.pop();
		for (auto it = cur->children->begin(); it != cur->children->end(); ++it) {
			traverse.push(*it);
		}
	}

}
