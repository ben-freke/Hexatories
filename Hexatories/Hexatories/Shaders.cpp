#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include "Log.h"
#include "Shaders.h"

using namespace std;

string readGLSL(const char *path) {

	string codeString;

	/*
		Fill out filepath for usage simplicity.
	*/
	string filePath = "Shaders\\";
	filePath.append(path);
	filePath.append(".glsl");

	ifstream file(filePath);	//check out fstream.

	if (file.is_open()) {

		string line;

		while (getline(file, line)) {
			codeString = codeString + line + "\n";
		}

		file.close();

	} else {
		log("Failed to load file: %s\n", filePath.c_str());
		return NULL;
	}

	return codeString;
}

bool checkCompile(GLuint shader) {
	int params;

	/*
		Get compile status for passed shader, if false get the error(s) and log it.
	*/
	glGetShaderiv(shader, GL_COMPILE_STATUS, &params);

	if (GL_TRUE != params) {

		int max_length = 2048;
		int actual_length = 0;
		char error[2048];

		glGetShaderInfoLog(shader, max_length, &actual_length, error);
		log("Shader info log for GL index %u:\n%s\n", shader, error);

		return false;
	}
	return true;
}

GLuint compileVShader(const char *path) {

	string code = readGLSL(path);
	const char *vs_code = code.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_code, NULL);
	glCompileShader(vs);

	if (!checkCompile(vs))
		return 0;

	return vs;
}

GLuint compileFShader(const char *path) {

	string code = readGLSL(path);
	const char *fs_code = code.c_str();

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_code, NULL);
	glCompileShader(fs);

	if (!checkCompile(fs))
		return 0;

	return fs;
}

GLuint createProgram(GLuint vs, GLuint fs) {
	GLuint shader_program = glCreateProgram();

	glAttachShader(shader_program, fs);
	glAttachShader(shader_program, vs);
	glLinkProgram(shader_program);

	/*
		Stuff below is essentially the same as compile checking for shaders, but for the program.
	*/
	int params;
	glGetProgramiv(shader_program, GL_LINK_STATUS, &params);
	if (GL_TRUE != params) {
		int max_length = 2048;
		int actual_length = 0;
		char plog[2048];

		glGetProgramInfoLog(shader_program, max_length, &actual_length, plog);
		log("program info log for GL index %u:\n%s", shader_program, plog);

		return 0;
	}

	return shader_program;
}