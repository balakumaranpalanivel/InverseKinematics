#pragma once
// Libraries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>


// Class Object3D
// This class represent a generic 3D object on the scene.
class Object3D {

protected:

	// -------------------- Attributes -------------------- //

	// Transformation matrix that convert local-space
	// coordinates to world-space coordinates
	glm::mat4 mTransformMatrix;

	bool isEuler = false;
public:

	// -------------------- Methods -------------------- //

	// Constructor
	Object3D();

	// Destructor
	virtual ~Object3D()
	{

	}

	// Return the transform matrix
	const glm::mat4& getTransformMatrix() const;

	// Set to the identity transform
	void setToIdentity();

	// Return the origin of object in world-space
	glm::vec3 getOrigin() const;

	// Translate the object in world-space
	void translateWorld(const glm::vec3& v);

	// Translate the object in local-space
	void translateLocal(const glm::vec3& v);

	// Scale the object in local-space
	void scaleLocal(glm::vec3 scaleVector);

	// Rotate the object in world-space
	//void rotateWorld(const glm::vec3& axis, float angle);

	// Rotate the object in local-space
	void rotateLocal(const glm::vec3& axis, float angle);

	// Rotate the obejct in local-space using quat
	void rotateLocalQuat(float rotateX,
		float rotateY, float rotateZ);

	// Rotate the obejct in local-space using quat
	void rotateLocalSpecial(float rotateX,
		float rotateY, float rotateZ);

	void Object3D::rotateLocalEuler(float rotateX, float rotateY, float rotateZ);

	//// Rotate around a world-space point
	//void rotateAroundWorldPoint(const glm::vec3& axis, float angle, const glm::vec3& point);

	//// Rotate around a local-space point
	//void rotateAroundLocalPoint(const glm::vec3& axis, float angle, const glm::vec3& worldPoint);

	//void SetChild(Object3D child);
};