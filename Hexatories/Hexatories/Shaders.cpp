#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <string>
#include "Log.h"
#include "Shaders.h"

using namespace std;

const char *readGLSL(const char *path) {

	string codeString;
	ifstream file(path);

	if (file.is_open()) {

		string line;
		while (getline(file, line)) {
			codeString = codeString + line;
		}

		file.close();

	} else {
		log("Failed to load file: %s", path);
		return NULL;
	}

	const char * code = codeString.c_str();

	log("Code: %s", code);
	return code;
}

GLuint compileVShader(const char *path) {
	const char *vs_code = readGLSL(path);

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_code, NULL);
	glCompileShader(vs);

	return vs;
}

GLuint compileFShader(const char *path) {
	const char *fs_code = readGLSL(path);

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_code, NULL);
	glCompileShader(fs);

	return fs;
}

GLuint createProgram(GLuint vs, GLuint fs) {
	GLuint shader_program = glCreateProgram();

	glAttachShader(shader_program, fs);
	glAttachShader(shader_program, vs);
	glLinkProgram(shader_program);

	return shader_program;
}