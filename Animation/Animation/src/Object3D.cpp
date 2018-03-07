#include "Object3D.h"

Object3D::Object3D()
{
	setToIdentity();
}

//
void Object3D::scaleLocal(glm::vec3 scaleVector)
{
	mTransformMatrix = glm::scale(mTransformMatrix, scaleVector);
}

// Return the transform matrix
inline const glm::mat4& Object3D::getTransformMatrix() const {
	return mTransformMatrix;
}

// Set to the identity transform
inline void Object3D::setToIdentity() {
	mTransformMatrix = glm::mat4();
}

// Return the origin of object in world-space
inline glm::vec3 Object3D::getOrigin() const {
	return mTransformMatrix * glm::vec4(0.0, 0.0, 0.0, 1.0);
}

// Translate the object in world-space
inline void Object3D::translateWorld(const glm::vec3& v) {
	mTransformMatrix = glm::translate(glm::mat4(), v) * mTransformMatrix;
}

// Translate the object in local-space
inline void Object3D::translateLocal(const glm::vec3& v) {
	mTransformMatrix = mTransformMatrix * glm::translate(glm::mat4(), v);
}

//// Rotate the object in world-space
//inline void Object3D::rotateWorld(const glm::vec3& axis, float angle) {
//	mTransformMatrix = Matrix4::rotationMatrix(axis, angle) * mTransformMatrix;
//}

// Rotate the object in local-space
inline void Object3D::rotateLocal(const glm::vec3& axis, float angle) {
	/*mTransformMatrix = mTransformMatrix * Matrix4::rotationMatrix(axis, angle);*/

	mTransformMatrix = mTransformMatrix * glm::rotate(glm::mat4(), angle, axis);

}

inline void Object3D::rotateLocalQuat(float rotateX,
	float rotateY, float rotateZ)
{
	glm::vec3 eulerAngles = glm::vec3(rotateX, rotateY,
		rotateZ);
	glm::quat MyQuaternion = glm::quat(eulerAngles);

	mTransformMatrix = mTransformMatrix * glm::toMat4(MyQuaternion);
}

inline void Object3D::rotateLocalEuler(float rotateX, float rotateY, float rotateZ)
{
	glm::mat4 X = glm::eulerAngleX(rotateX);
	glm::mat4 Y = glm::eulerAngleY(rotateY);
	glm::mat4 Z = glm::eulerAngleX(rotateZ);
	mTransformMatrix = mTransformMatrix * Y * X * Z;
}

void Object3D::rotateLocalSpecial(float rotateX, float rotateY, float rotateZ)
{
	if (isEuler)
	{
		rotateLocalEuler(rotateX, rotateY, rotateZ);
	}
	else
	{
		rotateLocalQuat(rotateX, rotateY, rotateZ);
	}
}

//// Rotate the object around a world-space point
//inline void Object3D::rotateAroundWorldPoint(const Vector3& axis, float angle,
//	const Vector3& worldPoint) {
//	mTransformMatrix = Matrix4::translationMatrix(worldPoint) * Matrix4::rotationMatrix(axis, angle)
//		* Matrix4::translationMatrix(-worldPoint) * mTransformMatrix;
//}

//// Rotate the object around a local-space point
//inline void Object3D::rotateAroundLocalPoint(const Vector3& axis, float angle,
//	const Vector3& worldPoint) {

//	// Convert the world point into the local coordinate system
//	Vector3 localPoint = mTransformMatrix.getInverse() * worldPoint;

//	mTransformMatrix = mTransformMatrix * Matrix4::translationMatrix(localPoint)
//		* Matrix4::rotationMatrix(axis, angle)
//		* Matrix4::translationMatrix(-localPoint);
//}

//void Object3D::SetChild(Object3D child)
//{
//
//}

