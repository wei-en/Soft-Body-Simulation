#include "texture.h"

#include "glad/glad.h"

#pragma warning(disable:4996)

namespace gfx
{
	unsigned int Texture::freeID = 0;

	void Texture::rgb_reverse()
	{
		unsigned int i, j;
		for (j = 0; j < height; j++)
		{
			for (i = 0; i < width; i++)
			{
				std::swap(data[(j * width + i) * 3], data[(j * width + i) * 3 + 2]);
			}
		}
	}

	Texture::Texture() {}

	Texture::Texture(unsigned int width, unsigned int height, unsigned char* inputData)
	{
		if (this->data)
			free(this->data);

		this->width = width;
		this->height = height;

		this->data = (unsigned char*)malloc(width * height * 3 * sizeof(unsigned char));

		if (inputData == NULL)
			memset(this->data, 0, width * height * 3 * sizeof(unsigned char));
		else
			memcpy(this->data, inputData, width * height * 3 * sizeof(unsigned char));
	}

	Texture::Texture(const char * fileName)
	{
		loadTextureFile(fileName);
	}

	unsigned int Texture::getID() const { return id; }
	unsigned char* Texture::getData() const { return data; }

	bool Texture::isEmpty() const { return (data == NULL); }
	
	void Texture::loadTextureFile(const char * fileName)
	{
		FILE *file;
		unsigned long size; // size of the image in bytes.
		unsigned long i; // standard counter.
		unsigned short int planes; // number of planes in image (must be 1)
		unsigned short int bpp; // number of bits per pixel (must be 24)

		if ((file = fopen(fileName, "rb")) == NULL)
		{
			printf("File Not Found : %s\n", fileName);
		}

		// seek through the bmp header, up to the width/height:
		fseek(file, 18, SEEK_CUR);
		// read the width
		if ((i = fread(&this->width, 4, 1, file)) != 1)
		{
			printf("Error reading width from %s.\n", fileName);
		}

		//printf("Width of %s: %lu\n", fileName, image->sizeX);
		// read the height
		if ((i = fread(&this->height, 4, 1, file)) != 1)
		{
			printf("Error reading height from %s.\n", fileName);
		}
		//printf("Height of %s: %lu\n", fileName, image->sizeY);
		// calculate the size (assuming 24 bits or 3 bytes per pixel).
		size = this->width * this->height * 3;
		// read the planes
		if ((fread(&planes, 2, 1, file)) != 1)
		{
			printf("Error reading planes from %s.\n", fileName);
		}
		if (planes != 1)
		{
			printf("Planes from %s is not 1: %u\n", fileName, planes);
		}
		// read the bitsperpixel
		if ((i = fread(&bpp, 2, 1, file)) != 1)
		{
			printf("Error reading bpp from %s.\n", fileName);
		}
		if (bpp != 24)
		{
			printf("Bpp from %s is not 24: %u\n", fileName, bpp);
		}
		// seek past the rest of the bitmap header.
		fseek(file, 24, SEEK_CUR);
		// read the data.
		this->data = (unsigned char *)malloc(size);
		if (this->data == NULL) {
			printf("Error allocating memory for color-corrected image data");
		}
		if ((i = fread(this->data, size, 1, file)) != 1) {
			printf("Error reading image data from %s.\n", fileName);
		}
		rgb_reverse();

		glEnable(GL_TEXTURE_2D);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		this->id = Texture::freeID;
		Texture::freeID++;

		glGenTextures(1, &(this->id));
		glBindTexture(GL_TEXTURE_2D, this->id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->width, this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, this->data);

		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
	}

	void Texture::saveAsFile(const char * fileName)
	{
		unsigned char bmp_file_header[14] = { 'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0, };
		unsigned char bmp_info_header[40] = { 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0, };
		unsigned char bmp_pad[3] = { 0, 0, 0, };

		const int size = 54 + width * height * 3;

		bmp_file_header[2] = static_cast<unsigned char>(size);
		bmp_file_header[3] = static_cast<unsigned char>(size >> 8);
		bmp_file_header[4] = static_cast<unsigned char>(size >> 16);
		bmp_file_header[5] = static_cast<unsigned char>(size >> 24);

		bmp_info_header[4] = static_cast<unsigned char>(width);
		bmp_info_header[5] = static_cast<unsigned char>(width >> 8);
		bmp_info_header[6] = static_cast<unsigned char>(width >> 16);
		bmp_info_header[7] = static_cast<unsigned char>(width >> 24);

		bmp_info_header[8] = static_cast<unsigned char>(height);
		bmp_info_header[9] = static_cast<unsigned char>(height >> 8);
		bmp_info_header[10] = static_cast<unsigned char>(height >> 16);
		bmp_info_header[11] = static_cast<unsigned char>(height >> 24);

		FILE *file = fopen(fileName, "wb");

		rgb_reverse();

		if (file)
		{
			fwrite(bmp_file_header, 1, 14, file);
			fwrite(bmp_info_header, 1, 40, file);

			for (unsigned int i = 0; i < height; i++)
			{
				fwrite(this->data + (width * i * 3), 3, width, file);
				fwrite(bmp_pad, 1, ((4 - (width * 3) % 4) % 4), file);
			}

			fclose(file);
		}

		rgb_reverse();
	}

	Texture::~Texture()
	{
		delete [] data;
	}
}