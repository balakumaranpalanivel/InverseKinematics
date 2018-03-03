#pragma once
#include <stdio.h>
#include <vector>

#include "Target.h"
#include "Segment.h"

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

const glm::vec3 refRotateVector(0.0f, 0.0f, -1.0f);

class Chain
{
public:

	Chain(vector<glm::vec3> joints, CTarget *t);
	Chain(glm::vec3 origin, glm::vec3 end, CTarget *t, int partitions = 5);
	
	void Render(glm::mat4 view, glm::mat4 proj);
	void Solve();
	// Second Endpoint becomes target and worked backwards
	void Backward();
	// First Endpoint becomes origin and worked forwards
	void Forward();

	glm::vec3 Constraint(
		glm::vec3 point,
		float true_length,
		CSegment *seg
	);

	void CalculateLinks(
		vector<glm::vec3> joints,
		vector<float> *lengths,
		vector<glm::quat> *direction
	);

	glm::vec3 GetFirstJoint();
	void SetFirstJoint(glm::vec3 joint);
	void SetSegments();
	void SetConstraint(vector<glm::vec4> constraint_list);

	unsigned long mSize;
	float mTotalLength;
	glm::vec3 mOrigin;
	glm::vec3 mEnd;
	CTarget *mTarget;
	bool please_constraint = false;

private:
	vector<glm::vec3> mJoints;
	vector<CSegment> mSegments;
	float mTolerance = 0.01f;
};