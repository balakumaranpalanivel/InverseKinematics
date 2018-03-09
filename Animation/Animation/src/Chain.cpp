#include "Chain.h"

Chain::Chain(vector<glm::vec3> joints, CEndEffector * t)
{

	vector<float> lengths;
	vector<glm::quat> orientations;
	CalculateLinks(joints, &lengths, &orientations);

	for (int i = 0; i < lengths.size(); ++i)
	{
		mLinks.push_back(
			CChainLink(
				joints[i], joints[i + 1],
				lengths[i],
				orientations[i])
		);
		total_length += lengths[i];
	}

	target = t;
	size = joints.size();
	this->joints = joints;

}

void Chain::SetConstraint(vector<glm::vec4> constraint_list)
{
	if (mLinks.size() != constraint_list.size())
	{
		cout << "Constraints expected :" << mLinks.size() << endl;
		cout << "Constraints given: " << constraint_list.size() << endl;
		return;
	}
	this->please_constraint = true;
	for (int i = 0; i < mLinks.size(); i++)
	{
		mLinks[i].mConstraintCone = constraint_list[i];
	}
}


Chain::Chain(glm::vec3 origin, glm::vec3 end, CEndEffector * t, int partitions)
{
	vector<float> lengths;
	vector<glm::quat> orientations;
	vector<glm::vec3> joints;

	glm::vec3 partition_checkpoint = origin;
	glm::vec3 dir = glm::normalize(end - origin);
	float magnitude = glm::length(end - origin);
	float partial_mag = magnitude / (float)partitions;

	// calculate the position of the different joints
	joints.push_back(origin);
	for (int i = 1; i <= partitions; ++i)
	{
		glm::vec3 to_insert = partition_checkpoint + partial_mag * i * dir;
		joints.push_back(to_insert);
	}

	// Determine the direction and length of each segment based on joint position
	CalculateLinks(joints, &lengths, &orientations);

	for (int i = 0; i < lengths.size(); ++i)
	{
		mLinks.push_back(
			CChainLink(
				joints[i], joints[i + 1],
				lengths[i],
				orientations[i])
		);
		total_length += lengths[i];
	}

	target = t;
	size = joints.size();
	this->joints = joints;
}

void Chain::Solve()
{
	// Find the distance from origin
	float current_distance = glm::length(target->mPosition - origin);

	// If target is out of reach - extend the arm fully
	if (current_distance > total_length)
	{
		for (int i = 0; i < joints.size() - 1; ++i)
		{
			float r = glm::length(target->mPosition - joints[i]);
			float l = mLinks[i].mMagnitude / r;
			joints[i + 1] = (1 - l) * joints[i] + l * target->mPosition;
		}

		vector<float> lengths;
		vector<glm::quat> orientations;
		CalculateLinks(joints, &lengths, &orientations);

	}
	else
	{
		// CEndEffector is in reach - Use FABRIK to solve
		int count = 0;

		// Find the difference between the target and the last point of the chain
		float difference = glm::length(
			joints[joints.size() - 1] - target->mPosition
		);

		// Solve for the angles till acceptable tolerance
		while (difference > tolerance || count < 1)
		{
			// Solve Backward
			Backward();

			// Solve Forward
			Forward();

			// Update the difference
			difference = glm::length(
				joints[joints.size() - 1] - target->mPosition
			);

			// Limit the retrying
			count++;
			if (count > 10)
			{
				break;
			}
		}
	}

	AssignLinks();

}

void Chain::AssignLinks()
{
	vector<float> lengths;
	vector<glm::quat> orientations;
	CalculateLinks(joints, &lengths, &orientations);

	for (int i = 0; i < lengths.size(); ++i)
	{
		mLinks[i].Set(
			joints[i], joints[i + 1],
			lengths[i], orientations[i]
		);
	}
}

// Make backward computation
void Chain::Backward()
{
	auto end = joints.end() - 1;

	// Make the end as the target position
	*end = target->mPosition;

	// Adjust each of remaining joints accordingly
	for (int i = int(joints.size() - 2); i >= 0; --i)
	{
		float r = glm::length(joints[i + 1] - joints[i]);
		float l = mLinks[i].mMagnitude / r;
		joints[i] = (1 - l) * joints[i + 1] + l * joints[i];
	}
}

// Make forward computation
void Chain::Forward()
{
	auto beg = joints.begin();

	// Set the beginning of the chain to origin
	*beg = origin;

	for (int i = 0; i < joints.size() - 1; ++i)
	{
		float r = glm::length(joints[i + 1] - joints[i]);
		float l = mLinks[i].mMagnitude / r;

		glm::vec3 new_point = (1 - l) * joints[i] + l * joints[i + 1];

		if (i > 0 && this->please_constraint)
		{
			new_point = Constraint(new_point, mLinks[i].mMagnitude, &(mLinks[i - 1]));
		}

		joints[i + 1] = new_point;
	}
}

// calculates the lengths and orientation of each joint given as input
void Chain::CalculateLinks(vector<glm::vec3> joints, vector<float> * lengths,
	vector<glm::quat> * orientations)
{
	// Set the beginning and end to the new joints
	origin = *joints.begin();
	end = *(joints.end() - 1);

	for (auto it = joints.begin(); it != joints.end() - 1; ++it)
	{
		glm::vec3 current = *it;
		glm::vec3 next = *(it + 1);

		glm::vec3 link_vector = next - current;

		// Get magnitude of link
		lengths->push_back(glm::length(link_vector));

		// Get rotation of link
		glm::vec3 cross = glm::cross(ref_rot_vector, link_vector);
		glm::quat q;
		q.x = cross.x;
		q.y = cross.y;
		q.z = cross.z;
		q.w = sqrt(
			pow(glm::length(link_vector), 2) * 
			pow(glm::length(ref_rot_vector), 2)) + 
			glm::dot(link_vector, ref_rot_vector);

		orientations->push_back(glm::normalize(q));
	}
}

glm::vec3 Chain::Constraint(glm::vec3 point, float true_length, CChainLink * pLink)
{
	glm::vec3 retval = point;
	glm::vec3 relative_point = point - pLink->mEndPosition;

	/*
		Get projection of the new calculated point onto the direction
		the previous segment is pointing in
	*/
	glm::vec3 line_dir = glm::normalize(pLink->GetConstraintConeAxis());
	float     scalar = glm::dot(point - pLink->mEndPosition, line_dir);
	glm::vec3 projected_point = scalar * line_dir + pLink->mEndPosition;


	glm::vec3 adjusted_distance = point - projected_point;
	if (scalar < 0)
	{
		glm::vec3 relative_projected_point = projected_point - pLink->mEndPosition;
		relative_projected_point = -relative_projected_point;
		projected_point = relative_projected_point + pLink->mEndPosition;
	}

	// Get the 2D axes we will be using for this problem
	glm::mat4 face_normals = pLink->GetFaceNormals();
	glm::vec3 up = glm::normalize(glm::vec3(face_normals[0]));
	glm::vec3 down = glm::normalize(glm::vec3(face_normals[1]));
	glm::vec3 left = glm::normalize(glm::vec3(face_normals[2]));
	glm::vec3 right = glm::normalize(glm::vec3(face_normals[3]));

	glm::vec3 x_axis = glm::length(left - relative_point) < glm::length(right - relative_point) ? left : right;
	glm::vec3 y_axis = glm::length(up - relative_point) < glm::length(down - relative_point) ? up : down;

	float x_aspect = glm::dot(adjusted_distance, x_axis);
	float y_aspect = glm::dot(adjusted_distance, y_axis);

	// Calculate the cone cross section
	float proj_length = glm::length(projected_point - pLink->mEndPosition);
	float up_cross = proj_length * glm::tan(
		glm::radians(pLink->mConstraintCone[0]));
	float down_cross = -(proj_length * glm::tan(
		glm::radians(pLink->mConstraintCone[1])));
	float left_cross = -(proj_length * glm::tan(
		glm::radians(pLink->mConstraintCone[2])));
	float right_cross = proj_length * glm::tan(
		glm::radians(pLink->mConstraintCone[3]));

	// See which quadrant we should use
	float x_bound = x_aspect >= 0 ? right_cross : left_cross;
	float y_bound = y_aspect >= 0 ? up_cross : down_cross;

	// Calculate where our point lies on the ellipse (finally)
	float ellipse_point = glm::pow(x_aspect, 2) / glm::pow(x_bound, 2) + glm::pow(y_aspect, 2) / glm::pow(y_bound, 2);

	// If the point we calculated is outside of this ellipse, then we must constraint the joint
	if (ellipse_point > 1 || scalar < 0)
	{
		float a = glm::atan(y_aspect, x_aspect);
		float x = x_bound * glm::cos(a);
		float y = y_bound * glm::sin(a);

		retval = glm::normalize((projected_point - pLink->mEndPosition) + (x_axis * x) + (y_axis * y)) * glm::length(relative_point) + pLink->mEndPosition;
	}

	return retval;
}

void Chain::Render(glm::mat4 view, glm::mat4 proj)
{
	for (auto it = mLinks.begin(); it != mLinks.end(); ++it)
	{
		it->Render(view, proj);
	}
}

glm::vec3 Chain::GetFirstJoint()
{
	return joints[0];
}

void Chain::SetFirstJoint(glm::vec3 joint)
{
	joints[0] = joint;
}
