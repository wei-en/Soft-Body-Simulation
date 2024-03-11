#include "color.h"

#include <assert.h>

namespace gfx
{
	// a is default to 1
	Color::Color(const float r, const float g, const float b, const float a)
	{
		// All of these RGBA component should be between 0 and 1, though we can manually clamp these values,
		// I'm not going to do it here. So that others can be more aware when they use it wrong.

		assert(r >= 0.0f && r <= 1.0f);
		assert(g >= 0.0f && g <= 1.0f);
		assert(b >= 0.0f && b <= 1.0f);
		assert(a >= 0.0f && a <= 1.0f);

		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	Color::~Color() {}
	// static instances for quick referencing
	Color Color::red = Color(1.0f, 0.0f, 0.0f, 1.0f);
	Color Color::green = Color(0.0f, 1.0f, 0.0f, 1.0f);
	Color Color::blue = Color(0.0f, 0.0f, 1.0f, 1.0f);
	Color Color::yellow = Color(1.0f, 1.0f, 0.0f, 1.0f);
	Color Color::orange = Color(1.0f, 0.54f, 0.1f, 1.0f);
	Color Color::white = Color(1.0f, 1.0f, 1.0f, 1.0f);
	Color Color::black = Color(0.0f, 0.0f, 0.0f, 1.0f);
	Color Color::none = Color(0.0f, 0.0f, 0.0f, 0.0f);
}