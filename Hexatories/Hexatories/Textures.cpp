#include <stdio.h>
#include <stdlib.h>
#include "Textures.h"
#include "Log.h"

unsigned char *loadBMP(const char *imagepath, int &width, int &height) {

	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;

	// Actual RGB data
	unsigned char * data;

	FILE *file;
	fopen_s(&file, imagepath, "rb");
	if (!file) return NULL;

	// Read the header
	// If less than 54 byes are read, problem
	if (fread(header, 1, 54, file) != 54) return NULL;

	// A BMP files always begins with "BM"
	if (header[0] != 'B' || header[1] != 'M') printf("Not a correct BMP file\n");

	// Make sure this is a 24bpp file
	if (*(int*)&(header[0x1E]) != 0) return NULL;
	if (*(int*)&(header[0x1C]) != 24) return NULL;

	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// If corrupt BMP
	if (imageSize == 0) imageSize = width * height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0) dataPos = 54; // The BMP header is done that way

	data = new unsigned char[imageSize];

	// Read pixels from the file into the buffer
	fread(data, 1, imageSize, file);

	fclose(file);

	return data;
}
