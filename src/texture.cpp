#include "texture.h"

// LoadBitmapFile
// desc: Returns a pointer to the TGA image of the TGA specified
//       by filename. Also returns the TGA header information.
//		 No support for 8-bit bitmaps.


// LoadTexture()
// desc: loads a texture given the filename
void CTexture::LoadTexture(char *filename)
{
	LoadTGATexture(filename);
}

// LoadTGAFile()
// desc: loads a TGA file defined by filename
unsigned char *CTexture::LoadTGAFile(char *filename, TGAHEADER *tgaHeader)
{
	FILE *filePtr;
	unsigned char ucharBad;	// garbage data
	short int	sintBad;		// garbage data
	long	imageSize;		// size of TGA image
	int colorMode;			// 4 for RGBA, 3 for RGB
	long imageIdx;			// counter variable
	unsigned char colorSwap;	// swap variable
	unsigned char *imageData;	// the TGA data

	// open the TGA file
	filePtr = fopen(filename, "rb");
	if (!filePtr)
		return NULL;
	
	// read first two bytes of garbage
	fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
	fread(&ucharBad, sizeof(unsigned char), 1, filePtr);

	// read in the image type
	fread(&tgaHeader->imageTypeCode, sizeof(unsigned char), 1, filePtr);

	// for our purposes, the image type should be either a 2 or a 3
	if ((tgaHeader->imageTypeCode != 2) && (tgaHeader->imageTypeCode != 3))
	{
		fclose(filePtr);
		return NULL;
	}

	// read 13 bytes of garbage data
	fread(&sintBad, sizeof(short int), 1, filePtr);
	fread(&sintBad, sizeof(short int), 1, filePtr);
	fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
	fread(&sintBad, sizeof(short int), 1, filePtr);
	fread(&sintBad, sizeof(short int), 1, filePtr);

	// read image dimensions
	fread(&tgaHeader->imageWidth, sizeof(short int), 1, filePtr);
	fread(&tgaHeader->imageHeight, sizeof(short int), 1, filePtr);

	// read bit depth
	fread(&tgaHeader->bitCount, sizeof(unsigned char), 1, filePtr);

	// read garbage
	fread(&ucharBad, sizeof(unsigned char), 1, filePtr);

	// colormode -> 3 = BGR, 4 = BGRA
	colorMode = tgaHeader->bitCount / 8;
	imageSize = tgaHeader->imageWidth * tgaHeader->imageHeight * colorMode;

	// allocate memory for image data
	imageData = (unsigned char*)malloc(sizeof(unsigned char)*imageSize);

	// read image data
	fread(imageData, sizeof(unsigned char), imageSize, filePtr);

	// change BGR to RGB so OpenGL can use the data
	for (imageIdx = 0; imageIdx < imageSize; imageIdx += colorMode)
	{
		colorSwap = imageData[imageIdx];
		imageData[imageIdx] = imageData[imageIdx+2];
		imageData[imageIdx + 2] = colorSwap;
	}

	// close the file
	fclose(filePtr);

	return imageData;
}

// LoadTGATexture()
// desc: loads a TGA as a texture
void CTexture::LoadTGATexture(char *filename)
{
	TGAHEADER tga;		// BMP header

	// store BMP data in texture
	data = LoadTGAFile(filename, &tga);
	if (data == NULL)
	{
		free(data);
	}
	
	// store texture information
	width = tga.imageWidth;
	height = tga.imageHeight;
	scaledHeight = 0;
	scaledWidth = 0;
	tgaImageCode = tga.imageTypeCode;
	bitDepth = tga.bitCount;
	textureType = TGA;
}