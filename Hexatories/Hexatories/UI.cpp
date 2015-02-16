#include "UI.h"
#include "Log.h"
#include "Shaders.h"
#include "Textures.h"

using namespace std;

void UI::initUI() {

}

void UI::drawUI() {

}

void UI::setupVAO(vector<GLushort> indices) {

	GLuint vs, fs, ebo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLint), verts.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

	GLuint tboMain, tboNums;

	loadBMP("gui.png", tboMain);
	loadBMP("nums.png", tboNums);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tboMain);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, tboNums);

	glActiveTexture(GL_TEXTURE0);

	vs = compileVShader("map_vs");
	fs = compileFShader("map_fs");
	prog = createProgram(vs, fs);

	uniforms[0] = glGetUniformLocation(prog, "main_tex");
	uniforms[1] = glGetUniformLocation(prog, "num_tex");

	GLint posAttrib = glGetAttribLocation(prog, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_INT, GL_FALSE, 8 * sizeof(GLint), 0);

	GLint colAttrib = glGetAttribLocation(prog, "colour");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_INT, GL_FALSE, 8 * sizeof(GLint), (void*)(2 * sizeof(GLint)));

	GLint texAttrib = glGetAttribLocation(prog, "tex_coord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 3, GL_INT, GL_FALSE, 8 * sizeof(GLint), (void*)(5 * sizeof(GLint)));

	glBindVertexArray(0);
}