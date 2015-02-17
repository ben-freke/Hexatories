#include "gameUI.h"
#include "Log.h"
#include "Shaders.h"
#include "Textures.h"

using namespace std;

void gameUI::initUI() {
	vector<GLushort> indices;
	mainOverlay(indices);
	initText(indices);

	setupVAO(indices);
}

void gameUI::drawUI() {
	glBindVertexArray(vao);

	glUseProgram(prog);

	for (int i = 0; i < 2; i++)
		glUniform1i(uniforms[i], i + 3);

	glDrawElements(GL_TRIANGLES, 222, GL_UNSIGNED_SHORT, 0);

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

	GLint rectVerts[] = {
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

	for (int i = 0; i < 20; i++) {
		verts.push_back(rectVerts[i]);
	}

}

void gameUI::initText(vector<GLushort> &indices) {

	GLint textVerts[] {
		5, 62, 598,			//coins
		4, 68, 525,			//pop
		5, 68, 456,			//total pop
		4, 5, 283,			//atk
		4, 5, 173,			//total atk
		4, 77, 283,			//def
		4, 77, 173,			//total def
		3, 907, 565,		//send atk
		3, 963, 565,
	};

	GLushort textIndices[] = {
			0, 1, 2,
			2, 3, 0,
	};

	GLushort baseIndex = 4;

	for (int i = 0; i < 27; i+=3) {	// 9 different locations * 3 = 27
		for (int j = 0; j < textVerts[i]; j++) { // number of letters at location
			for (int k = 0; k < 2; k++) { // 2x2 square
				for (int l = 0; l < 2; l++) {

					int kxorl = (k || l) &! (k && l);
					verts.push_back(textVerts[i + 1] + ((j + kxorl) * 14));
					verts.push_back(textVerts[i + 2] - 16 * k);
					verts.push_back(kxorl);
					verts.push_back(k);
					verts.push_back(0);
				}
			}

			for (int k = 0; k < 6; k++)
				indices.push_back(baseIndex + textIndices[k]);

			baseIndex += 4;
		}
	}

}