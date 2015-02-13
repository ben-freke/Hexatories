#include <stdio.h>
#include "SOIL.h"
#include "Textures.h"

using namespace std;

void loadBMP(const char *imagePath, GLuint &tboTex) {

	int width, height;

	unsigned char *data = SOIL_load_image(imagePath, &width, &height, 0, SOIL_LOAD_RGBA);

	glGenTextures(1, &tboTex);
	glBindTexture(GL_TEXTURE_2D, tboTex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	SOIL_free_image_data(data);
}