#include "MultiChain.h"

MultiChain::MultiChain(vector<Chain*> chains)
{
	mRoot = new ChainNode();
	mRoot->value = *chains.begin();
	mRoot->parent = NULL;
	mRoot->children = new vector<ChainNode*>();
	bool success = false;

	mOrigin = mRoot->value->mOrigin;

	for (auto it = chains.begin() + 1; it != chains.end(); ++it)
	{
		Chain* current = *it;
		success = true && Insert(mRoot, current);
	}

	if (!success)
	{
		cerr << "Something went wrong when constructing the multichain; one of the nodes probably didn't match up." << endl;
		exit(-1);
	}
}

bool MultiChain::Insert(ChainNode* root, Chain *chain)
{
	if (glm::all(glm::equal(mRoot->value->mEnd, chain->mOrigin)))
	{
		ChainNode * new_node = new ChainNode();
		new_node->value = chain;
		new_node->parent = root;
		new_node->children = new vector<ChainNode*>();

		root->children->push_back(new_node);
		root->value->mTarget = new CTarget(0, 0, 0);

		// if this node is leaf. Set flag
		if (mLeaves[root])
		{
			mLeaves[root] = false;
		}

		return true;
	}
	else if (root->children->size())
	{
		bool was_inserted = false;
		for (auto it = root->children->begin();
			it != root->children->end();
			++it)
		{
			ChainNode* recurse = *it;
			was_inserted = was_inserted || Insert(recurse, chain);
			if (was_inserted)
			{
				break;
			}
		}
	}

	return false;
}

void MultiChain::Solve()
{
	mRoot->value->mOrigin = mOrigin;
	mRoot->value->SetFirstJoint(mOrigin);

	Backward(mRoot);
	mRoot->value->Solve();
	Forward(mRoot);
}

void MultiChain::Backward(ChainNode *root)
{
	if (root->children->size())
	{
		for (auto it = root->children->begin(); it != root->children->end(); ++it)
		{
			Backward(*it);
		}

		// Calculate the centroid
		glm::vec3 centroid;
		int i = 0;
		for (auto it = root->children->begin(); it != root->children->end(); ++it)
		{
			ChainNode * node = *it;
			centroid += node->value->GetFirstJoint();
			++i;
		}
		centroid = centroid / (float)i;

		// Set the centroid to the target; this should solve toward the centroid now

		//if(glm::length(centroid - root->value->origin) < root->value->total_length)
		root->value->mTarget->mPosition = centroid;
	}

	if (root->parent)
	{
		root->value->Backward();
	}
}

void MultiChain::Forward(ChainNode * root)
{
	if (root->children->size())
	{
		for (auto it = root->children->begin(); it != root->children->end(); ++it)
		{
			Forward(*it);
		}
	}

	if (root->parent)
	{
		root->value->mOrigin = root->parent->value->mEnd;
		root->value->Forward();
		root->value->SetSegments();
	}
}

void MultiChain::Render(glm::mat4 view, glm::mat4 proj)
{

	// Traverse tree and render each chain. Doesn't really matter in what order
	// Not using recursion because don't want to add an unnecessary parameter lol
	stack<ChainNode*> traverse;
	traverse.push(mRoot);

	while (!traverse.empty())
	{
		ChainNode * cur = traverse.top();
		cur->value->Render(view, proj);
		traverse.pop();
		for (auto it = cur->children->begin(); it != cur->children->end(); ++it)
		{
			traverse.push(*it);
		}
	}

}

