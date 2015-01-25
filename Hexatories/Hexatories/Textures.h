#ifndef __TEXTURES_H
#define __TEXTURES_H

	/*
		Returns the rgb information from a bitmap and stores the width & height in the passed vars.
	*/
	unsigned char *loadBMP(const char *imagepath, int &outWidth, int &outHeight);
#endif