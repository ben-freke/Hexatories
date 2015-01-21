#include <string>
#ifndef __SHADERS_H
#define __SHADERS_H

	// All shaders should be saved in /Shaders/ and have filetype .glsl
	// For example /Shaders/test_vs.glsl
	// File path in this case would just be "test_vs"

	/*example usage
	GLuint vs = compileVShader("test_vs");
	GLuint fs = compileFShader("test_fs");
	GLuint prog = createProgram(vs, fs);

	glUseProgram(prog);
	
	To change shader just call glUseProgram again, or to stop using any use 0 as the param.
	*/

	std::string readGLSL(const char *path);
	GLuint compileVShader(const char *path);
	GLuint compileFShader(const char *path);
	GLuint createProgram(GLuint vs, GLuint fs);
#endif