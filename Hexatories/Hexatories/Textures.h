#include <GL\glew.h>
#include <GLFW/glfw3.h>
#ifndef __TEXTURES_H
#define __TEXTURES_H

	/*
		Returns the rgb information from a bitmap and stores the width & height in the passed vars.
	*/
	void loadBMP(const char *, GLuint &);
#endif