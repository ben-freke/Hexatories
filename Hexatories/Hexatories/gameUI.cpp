#include "gameUI.h"
#include "Log.h"
#include "Shaders.h"
#include "Textures.h"

using namespace std;

void gameUI::initUI() {
	vector<GLushort> indices;
	mainOverlay(indices);

	setupVAO(indices);
}

void gameUI::drawUI() {
	glBindVertexArray(vao);

	glUseProgram(prog);

	for (int i = 0; i < 1; i++)
		glUniform1i(uniforms[i], i + 3);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);
}

void gameUI::setupVAO(vector<GLushort> indices) {

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

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, tboMain);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, tboNums);

	glActiveTexture(GL_TEXTURE0);

	vs = compileVShader("main_vs");
	fs = compileFShader("UI_fs");
	prog = createProgram(vs, fs);

	uniforms[0] = glGetUniformLocation(prog, "main_tex");
	uniforms[1] = glGetUniformLocation(prog, "num_tex");

	GLint posAttrib = glGetAttribLocation(prog, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_INT, GL_FALSE, 5 * sizeof(GLint), 0);

	GLint texAttrib = glGetAttribLocation(prog, "tex_coord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 3, GL_INT, GL_FALSE, 5 * sizeof(GLint), (void*)(2 * sizeof(GLint)));

	glBindVertexArray(0);
}

void gameUI::mainOverlay(vector<GLushort> &indices) {

	GLint tileVertsTex[] = {
		0, 768, 0, 0, 10,
		1024, 768, 1, 0, 10,
		1024, 0, 1, 1, 10,
		0, 0, 0, 1, 10,
	};

	GLushort rectIndices[] = {
		0, 1, 2,
		2, 3, 0,

	};
	/*
	Adds these vertices to the end of the vbo and the indices to the ebo.
	*/
	for (int i = 0; i < 6; i++) {
		indices.push_back(rectIndices[i]);
	}

	for (int i = 0; i < 32; i++) {
		verts.push_back(tileVertsTex[i]);
	}

}