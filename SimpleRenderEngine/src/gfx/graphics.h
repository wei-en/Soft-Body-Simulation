#pragma once

#include <array>

#include "color.h"
#include "texture.h"

namespace gfx
{
	/**
	 *  graphics class for rendering different primitives(point, line segment, spherem, cube...etc)
	 *
	 *  1. Should only be accessed by static method calls and should not have any instance(s).
	 *
	 *  2. Should not depend on any other libraries except OpenGL related ones.
	 *     So the parameter types are native c++ classes, not a more convenient vector class.
	 *
	 *  3. Should not know anything about setting up callback function and creating windows.
	 *     So do not put initialization and callback registering code in here.
	 */
	class Graphics final
	{
	private:
		// remove constructor, so only static usage is left.
		Graphics() = delete;

		static Color s_currentColor;
	public:
		// Frame setting up and finishing
		static void startFrame(const std::array<float, 4>& clearColor);

		static void finishFrame();
		// Color and Texture
		static void setColor(const Color& color);

		static void setTexture(const Texture& texture);

		static void setTextureToNull();
		// Render geometry
		static void renderPoint(const std::array<float, 3>& position, const float& size);

		static void renderLineSegment(const std::array<float, 3>& start, const std::array<float, 3>& end);

		static void renderPlane(const std::array<float, 3>& min_position, const std::array<float, 3>& max_position, const float uvScalar = 1.0f);

		static void renderSphere(const std::array<float, 3>& position, const float& radius);

		static void renderWireSphere(const std::array<float, 3>& position, const float& radius);

		static void renderCube(const std::array<float, 3>& position, const float& size);

		static void renderCylinder(const std::array<float, 3>& start, const std::array<float, 3>& end, const float& radius);
	};
}