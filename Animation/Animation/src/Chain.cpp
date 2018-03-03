#include "Chain.h"

Chain::Chain(vector<glm::vec3> joints, CTarget *t)
{
	vector<float> lengths;
	vector<glm::quat> directions;
	CalculateLinks(joints, &lengths, &directions);

	for (int i = 0; i < lengths.size(); ++i)
	{
		mSegments.push_back(
			CSegment(joints[i], joints[i + 1],
				lengths[i], directions[i]));
		mTotalLength += lengths[i];
	}

	mTarget = t;
	mSize = joints.size();
	this->mJoints = joints;
}

void Chain::SetConstraint(vector<glm::vec4> constraint_list)
{
	if (mSegments.size() != constraint_list.size())
	{
		cout << "Constrains expected :" << mSegments.size() << endl;
		cout << "Constrains given: " << constraint_list.size() << endl;
		return;
	}

	for (int i = 0; i < mSegments.size(); i++)
	{
		mSegments[i].mConstraintCone = constraint_list[i];
	}
}

Chain::Chain(glm::vec3 origin, glm::vec3 end, CTarget *t, int partitions)
{
	vector<float> lengths;
	vector<glm::quat> directions;
	vector<glm::vec3> joints;

	glm::vec3 partition_checkpoint = origin;
	glm::vec3 dir = glm::normalize(end - origin);
	float magnitude = glm::length(end - origin);
	float partial_mag = magnitude / (float)partitions;

	// calculate the position of the different joints
	joints.push_back(origin);
	for (int i = 1; i <= partitions; ++i)
	{
		glm::vec3 toInsert = partition_checkpoint + partial_mag * i * dir;
		joints.push_back(toInsert);
	}

	// Determine the direction and length of each segment based on joint position
	CalculateLinks(joints, &lengths, &directions);
	for (int i = 0; i < lengths.size(); ++i)
	{
		mSegments.push_back(
			CSegment(joints[i], joints[i+1],
				lengths[i], directions[i])
		);
		mTotalLength += lengths[i];
	}

	mTarget = t;
	mSize = joints.size();
	this->mJoints = joints;
}

void Chain::Solve()
{
	// Find the distance from origin
	float current_distance = glm::length(mTarget->mPosition - mOrigin);

	// If target is out of reach - extend the arm fully
	if (current_distance > mTotalLength)
	{
		for (int i = 0; i < mJoints.size() - 1; ++i)
		{
			float r = glm::length(mTarget->mPosition - mJoints[i]);
			float l = mSegments[i].mMagnitude / r;
			mJoints[i + 1] = (1 - l) * mJoints[i] + l * mTarget->mPosition;
		}

		vector<float> lengths;
		vector<glm::quat> directions;
		CalculateLinks(mJoints, &lengths, &directions);
	}
	else
	{
		// Target is in reach - Use FABRIK to solve
		int count = 0;

		// Find the difference between the target and the last point of the chain
		float difference = glm::length(
			mJoints[mJoints.size() - 1] - mTarget->mPosition
		);

		// Solve for the angles till acceptable tolerance
		while (difference > mTolerance || count < 1)
		{
			// Solve Backward
			Backward();

			// Solve Forward
			Forward();

			// Update the difference
			difference = glm::length(
				mJoints[mJoints.size() - 1] - mTarget->mPosition);

			count++;

			// Limit the retrying
			if (count > 10)
			{
				break;
			}
		}
	}

	// Update the segments based on new joints
	SetSegments();
}

void Chain::SetSegments()
{
	vector<float> lengths;
	vector<glm::quat> directions;
	CalculateLinks(mJoints, &lengths, &directions);

	for (int i = 0; i < lengths.size(); ++i)
	{
		mSegments[i].Set(
			mJoints[i], mJoints[i+1],
			lengths[i], directions[i]
		);
	}
}

// Make backward computation
void Chain::Backward()
{
	auto end = mJoints.end() - 1;

	// Make the end as the target position
	*end = mTarget->mPosition;

	// Adjust each of remaining joints accordingly
	for (int i = int(mJoints.size() - 2); i >= 0; --i)
	{
		float r = glm::length(mJoints[i + 1] - mJoints[i]);
		float l = mSegments[i].mMagnitude / r;
		mJoints[i] = (1 - l)*mJoints[i + 1] + l * mJoints[i];
	}
}

// Make forward computation
void Chain::Forward()
{
	auto beg = mJoints.begin();
	
	// Set the beginning of the chain to origin
	*beg = mOrigin;

	for (int i = 0; i < mJoints.size() - 1; i++)
	{
		float r = glm::length(mJoints[i + 1] - mJoints[i]);
		float l = mSegments[i].mMagnitude / r;

		glm::vec3 new_point = (1 - l)*mJoints[i] + l * mJoints[i + 1];

		if (i > 0 && this->please_constraint)
		{
			new_point = Constraint(new_point, mSegments[i].mMagnitude,
				&(mSegments[i - 1]));
		}

		mJoints[i + 1] = new_point;
	}
}

// calculates the lengths and orientation of each joint given as input
void Chain::CalculateLinks(vector<glm::vec3> joints, vector<float> *lengths,
	vector<glm::quat> *directions)
{
	// Set the beginning and end to the new joints
	mOrigin = *joints.begin();
	mEnd = *(joints.end() - 1);

	for (auto it = joints.begin(); it != joints.end() - 1; ++it)
	{
		glm::vec3 current = *it;
		glm::vec3 next = *(it + 1);

		glm::vec3 link_vector = next - current;

		// Get magnitude of link
		lengths->push_back(glm::length(link_vector));

		// Get rotation of link
		glm::vec3 cross = glm::cross(refRotateVector, link_vector);
		glm::quat q;
		q.x = cross.x;
		q.y = cross.y;
		q.z = cross.z;
		q.w = sqrt(
			pow(glm::length(link_vector), 2) * 
				pow(glm::length(refRotateVector), 2) +
			glm::dot(link_vector, refRotateVector)
		);

		directions->push_back(glm::normalize(q));
	}
}

glm::vec3 Chain::Constraint(glm::vec3 point, float true_length, CSegment * seg)
{

	glm::vec3 retval = point;
	glm::vec3 relative_point = point - seg->mEndPosition;

	bool debug = false;

	/*
		Get projection of the new calculated point onto the direction
		the previous segment is pointing in
	*/
	glm::vec3 line_dir = glm::normalize(seg->GetConstraintConeAxis());
	float     scalar = glm::dot(point - seg->mEndPosition, line_dir);
	glm::vec3 projected_point = scalar * line_dir + seg->mEndPosition;


	if (debug)
	{
		float angle = 
			glm::acos(scalar / (glm::length(point - seg->mEndPosition) * glm::length(line_dir)));
		cout << "angle: " << glm::degrees(angle) << endl;
		cout << "scalar: " << scalar << endl;
		cout << "projected point: " << projected_point.x << " " 
			<< projected_point.y << " " << projected_point.z << " " << endl;
	}

	glm::vec3 adjusted_distance = point - projected_point;
	if (scalar < 0)
	{
		glm::vec3 relative_projected_point = projected_point - seg->mEndPosition;
		relative_projected_point = -relative_projected_point;
		projected_point = relative_projected_point + seg->mEndPosition;
	}

	// Get the 2D axes we will be using for this problem
	glm::mat4 face_normals = seg->GetFaceNormals();
	glm::vec3 up = glm::normalize(glm::vec3(face_normals[0]));
	glm::vec3 down = glm::normalize(glm::vec3(face_normals[1]));
	glm::vec3 left = glm::normalize(glm::vec3(face_normals[2]));
	glm::vec3 right = glm::normalize(glm::vec3(face_normals[3]));

	glm::vec3 x_axis = glm::length(left - relative_point) < glm::length(right - relative_point) ? left : right;
	glm::vec3 y_axis = glm::length(up - relative_point) < glm::length(down - relative_point) ? up : down;

	float x_aspect = glm::dot(adjusted_distance, x_axis);
	float y_aspect = glm::dot(adjusted_distance, y_axis);

	if (debug)
	{
		cout << glm::radians(seg->mConstraintCone[0]) << endl;
		cout << glm::radians(seg->mConstraintCone[0]) << endl;
		cout << glm::tan(glm::radians(seg->mConstraintCone[0])) << endl;
		cout << "x axis: " << x_axis.x << " " << x_axis.y << " " << x_axis.z << endl;
	}

	// Calculate the cone cross section
	float proj_length = glm::length(projected_point - seg->mEndPosition);
	float up_cross = proj_length * glm::tan(
		glm::radians(seg->mConstraintCone[0]));
	float down_cross = -(proj_length * glm::tan(
		glm::radians(seg->mConstraintCone[1])));
	float left_cross = -(proj_length * glm::tan(
		glm::radians(seg->mConstraintCone[2])));
	float right_cross = proj_length * glm::tan(
		glm::radians(seg->mConstraintCone[3]));

	// See which quadrant we should use
	float x_bound = x_aspect >= 0 ? right_cross : left_cross;
	float y_bound = y_aspect >= 0 ? up_cross : down_cross;

	// Calculate where our point lies on the ellipse (finally)
	float ellipse_point = glm::pow(x_aspect, 2) / glm::pow(x_bound, 2) + glm::pow(y_aspect, 2) / glm::pow(y_bound, 2);

	//if(debug) cout << "ellipse point: " << ellipse_point << endl;
	cout << "y_aspect " << y_aspect << "  x_aspect " << x_bound << endl;

	// If the point we calculated is outside of this ellipse, then we must constrain the joint
	if (ellipse_point > 1 || scalar < 0) {
		cout << "Not in bounds!" << endl;
		float a = glm::atan(y_aspect, x_aspect);
		float x = x_bound * glm::cos(a);
		float y = y_bound * glm::sin(a);

		retval = glm::normalize((projected_point - seg->mEndPosition) + (x_axis * x) + (y_axis * y)) * glm::length(relative_point) + seg->mEndPosition;
	}
	else {
		cout << "In bounds!" << endl;
	}

	return retval;
}

void Chain::Render(glm::mat4 view, glm::mat4 proj)
{
	for (auto it = mSegments.begin(); it != mSegments.end(); ++it)
	{
		it->Render(view, proj);
	}
}

glm::vec3 Chain::GetFirstJoint()
{
	return mJoints[0];
}

void Chain::SetFirstJoint(glm::vec3 joint)
{
	mJoints[0] = joint;
}