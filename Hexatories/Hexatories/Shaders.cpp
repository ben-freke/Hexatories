#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <string>
#include "Log.h"
#include "Shaders.h"

using namespace std;

string readGLSL(const char *path) {

	string codeString;

	string filePath = "Shaders\\";
	filePath.append(path);
	filePath.append(".glsl");

	ifstream file(filePath);

	if (file.is_open()) {

		string line;
		while (getline(file, line)) {
			codeString = codeString + line + "\n";
			log("Read line %s\n", line.c_str());
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
	log("Code: %s\n", vs_code);

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
	log("Code: %s\n", fs_code);

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

	return shader_program;
}