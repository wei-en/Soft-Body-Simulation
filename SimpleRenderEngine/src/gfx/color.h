#pragma once

namespace gfx
{
	struct Color
	{
	public:
		float r, g, b, a;

		static Color red;
		static Color green;
		static Color blue;
		static Color yellow; 
		static Color orange;
		static Color white;
		static Color black;
		static Color none;

		Color(const float r, const float g, const float b, const float a = 1.0f);
		~Color();
	};
}