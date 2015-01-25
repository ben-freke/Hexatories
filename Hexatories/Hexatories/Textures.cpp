#include <stdio.h>
#include <stdlib.h>
#include "Textures.h"
#include "Log.h"

unsigned char *loadBMP(const char *imagePath, int &width, int &height) {

	/*
		header - the bitmap header
	*/
	unsigned char header[54];

	FILE *file;
	fopen_s(&file, imagePath, "rb");

	if (!file) {
		log("Invalid bitmap filepath: %s\n", imagePath);
		return NULL;
	}

	/*
		If we read less that 54 bytes then the header is corrupt at the very least, ABORT ABORT
	*/
	if (fread(header, 1, 54, file) != 54) {
		log("Read less than 54 bytes from: %s\n", imagePath);
		return NULL;
	}
	/*
		The following ensure from the header that we have the right type of bmp.
	*/
	if (header[0] != 'B' || header[1] != 'M') {
		log("Header invalid, corrupt bmp: %s\n", imagePath);
	}
	
	if (*(int*)&(header[0x1E]) != 0) {
		log("Not a rgb bitmap: %s\n", imagePath);
		return NULL;
	}

	if (*(int*)&(header[0x1C]) != 24) {
		log("Not a 24bpp bitmap: %s\n", imagePath);
		return NULL;
	}

	/*
		Get stuff from the header.
	*/
	unsigned int imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	/*
		imageSize being 0 doesn't mean we have a corrupt bmp, but we do need it.
	*/
	if (imageSize == 0) imageSize = width * height * 3; // 3 - one byte for each rgb component

	unsigned char *data = new unsigned char[imageSize];

	fread(data, 1, imageSize, file);

	fclose(file);

	return data;
}
