#pragma once

#include <stdio.h>
#include <vector>

#include "EndEffector.h"
#include "ChainLink.h"

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

const glm::vec3 ref_rot_vector(0.0f, 0.0f, -1.0f);

class Chain
{
public:
	
	Chain(vector<glm::vec3> joints, CEndEffector * t);
	Chain(glm::vec3 origin, glm::vec3 end, CEndEffector * t, int partitions = 5);
	
	void Render(glm::mat4 view, glm::mat4 proj);
	void Solve();

	// Second Endpoint becomes target and worked backwards	
	void Backward(); 
	// First Endpoint becomes origin and worked forwards
	void Forward();  

	glm::vec3 Constraint(
		glm::vec3 point,
		float true_length,
		Segment * seg);

	void CalculateLinks(
		vector<glm::vec3> joints,
		vector<float> * lengths,
		vector<glm::quat> * directions);

	glm::vec3 GetFirstJoint();
	void SetFirstJoint(glm::vec3 joint);
	void SetSegments();
	void SetConstraint(vector<glm::vec4> constraint_list);

	unsigned long size;
	float total_length;
	glm::vec3 origin;
	glm::vec3 end;
	CEndEffector * target;
	bool please_constraint = false;

private:
	vector<glm::vec3> joints; // Joints themselves
	vector<Segment> segments; // The pieces that actually get rendered
	float tolerance = 0.01f;

};
