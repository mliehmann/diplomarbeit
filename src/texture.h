#ifndef __TEXTURE_H
#define __TEXTURE_H

/*
	TEXTURE.H

	The CTexture class
	OpenGL Game Programming
	Author: Kevin Hawkins (changed by Markus Liehmann: removed BMP & PCX support, removed glDeleteTextures)
	Date: 3/29/2001
	Description: The CTexture class represents a single texture
			   object in the engine. To load a texture,
			   you only need to call the LoadTexture() function.

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

enum texTypes_t
{
	PCX,
	BMP,
	TGA
};

typedef struct
{
	unsigned char imageTypeCode;
	short int imageWidth;
	short int imageHeight;
	unsigned char bitCount;
} TGAHEADER;

class CTexture
{
private:
	long int scaledWidth;
	long int scaledHeight;

	unsigned char *palette;

	unsigned char *LoadTGAFile(char *filename, TGAHEADER *tgaHeader);
	void LoadTGATexture(char *filename);

public:
	texTypes_t textureType;
	unsigned char tgaImageCode;	// 0 = not TGA image, 2 = color, 3 = greyscale

	int width;
	int height;
	int bitDepth;
	unsigned int texID;

	unsigned char *data;

	CTexture() { data = NULL; texID = 0;}
	~CTexture()
	{
		FreeData();
	}

	void LoadTexture(char *filename);

	void FreeData()
	{
		if (data != NULL)
			free(data);
		
		data = NULL;
	}
};

#endif