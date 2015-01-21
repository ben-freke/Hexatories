#ifndef __SHADERS_H
#define __SHADERS_H

	const char *readGLSL(const char *path);
	GLuint compileVShader(const char *path);
	GLuint compileFShader(const char *path);
	GLuint createProgram(GLuint vs, GLuint fs);
#endif