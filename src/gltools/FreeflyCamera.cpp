#include "FreeflyCamera.h"

#define _USE_MATH_DEFINES
#include <cmath>

GLTools::FreeflyCamera::FreeflyCamera() : GLTools::Camera3D(), mPitch(0.0f), mRoll(0.0f), mYaw(0.0f) {

}

glm::mat4 GLTools::FreeflyCamera::getViewMatrix() const {
	glm::mat4 viewMatrix = glm::mat4(1.0f);

	viewMatrix = glm::rotate(viewMatrix, mPitch, glm::vec3(1.0f, 0.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, mYaw, glm::vec3(0.0f, 1.0f, 0.0f));
	viewMatrix = glm::translate(viewMatrix, mPosition);

	//viewMatrix = glm::rotate(viewMatrix, mPitch, glm::vec3(1.0f, 0.0f, 0.0f));
	//viewMatrix = glm::rotate(viewMatrix, mRoll, glm::vec3(0.0f, 0.0f, 1.0f));

	return viewMatrix;
}

glm::mat4 GLTools::FreeflyCamera::getMVMatrix() const {
	return getViewMatrix() * Camera3D::getMVMatrix();
}

glm::mat4 GLTools::FreeflyCamera::getNormalMatrix() const {
	return glm::transpose(glm::inverse(getMVMatrix()));
}

void GLTools::FreeflyCamera::moveLeft(float t) {
	glm::vec3 mFront = glm::vec3(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) * glm::rotate(glm::mat4(1.0f), mYaw, glm::vec3(0.0f, 1.0f, 0.0f))) * t;
	mPosition += mFront;
}

void GLTools::FreeflyCamera::moveFront(float t) {
	glm::vec3 mFront = glm::vec3(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f) * glm::rotate(glm::mat4(1.0f), mYaw, glm::vec3(0.0f, 1.0f, 0.0f))) * t;
	mPosition += mFront;
}

void GLTools::FreeflyCamera::rotateLeft(float t) {
	mYaw = mYaw + t;
}

void GLTools::FreeflyCamera::rotateUp(float t) {
	mPitch = mPitch - t;
}

void GLTools::FreeflyCamera::moveUp(float t) {
	mPosition.y = mPosition.y + t;
}
