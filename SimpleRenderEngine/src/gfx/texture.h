#pragma once

#include <vector>

namespace gfx
{
	class Texture
	{
	private:
		// this is for recording which id is free to assign to a new texture
		static unsigned int freeID;

		unsigned int id;

		unsigned int width, height;
		unsigned char* data;

		void rgb_reverse();

	public:
		Texture();
		Texture(unsigned int width, unsigned int height, unsigned char* inputData = NULL);
		Texture(const char* fileName);

		unsigned int getID() const;
		unsigned char* getData() const;

		bool isEmpty() const;

		void loadTextureFile(const char* fileName);
		void saveAsFile(const char* fileName);

		~Texture();
	};
}