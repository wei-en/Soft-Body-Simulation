#include "camera.h"

#include "GL/freeglut.h"

namespace gfx
{
	Camera::Camera(const float fieldOfView, const float nearClipPlane, const float farClipPlane)
		: fieldOfView(fieldOfView), nearClipPlane(nearClipPlane), farClipPlane(farClipPlane) {}

	Camera::~Camera() {}

	void Camera::setAspectRatio(const float aspectRatio)
	{
		this->aspectRatio = aspectRatio;
		this->isUsingScreenAspectRatio = false;
	}

	float Camera::getAspectRatio() const
	{
		return (this->isUsingScreenAspectRatio ? (glutGet(GLUT_WINDOW_WIDTH) / glutGet(GLUT_WINDOW_HEIGHT)) : this->aspectRatio);
	}
}