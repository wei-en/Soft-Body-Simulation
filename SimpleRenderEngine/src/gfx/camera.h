#pragma once

#include <array>

namespace gfx
{
	/**
	 *  This camera class is inspired by the Unity's implementation of a virtual camera.
	 *  
	 *  
	 */
	class Camera
	{
	private:
		bool isUsingScreenAspectRatio = true;
		float aspectRatio; // this should be window width / window height, though not necessary

	public:
		float fieldOfView;
		float nearClipPlane;
		float farClipPlane;

		std::array<float, 3> position;
		std::array<float, 3> up;
		std::array<float, 3> lookAt;

		Camera(const float fieldOfView = 60.0f, const float nearClipPlane = 0.01f, const float farClipPlane = 100.0f);
		~Camera();

		void setAspectRatio(const float aspectRatio);
		float getAspectRatio() const;
	};
}