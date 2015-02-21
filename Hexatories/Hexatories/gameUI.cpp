#include <string>
#include "gameUI.h"
#include "Log.h"
#include "Shaders.h"
#include "Textures.h"

using namespace std;

void gameUI::initUI() {
	mainOverlay();
	initText();

	setupVAO();
}

void gameUI::drawUI() {
	glBindVertexArray(vao);

	glUseProgram(prog);

	for (int i = 0; i < 3; i++)
		glUniform1i(uniforms[i], i + 4);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);
}

void gameUI::changeButton(int button) {
	static int pos[2];

	GLint rectVerts[] = {
		917, 476, 0, 0, button + 11,
		998, 476, 1, 0, button + 11,
		998, 430, 1, 1, button + 11,
		917, 430, 0, 1, button + 11,
	};

	GLushort rectIndices[] = {
		0, 1, 2,
		2, 3, 0,
	};

	if (button > -1) {
		if (pos[0] == 0) {
			pos[0] = indices.size();
			pos[1] = verts.size();

			for (int i = 0; i < 6; i++) indices.push_back((pos[1] / 5) + rectIndices[i]);
			for (int i = 0; i < 20; i++) verts.push_back(rectVerts[i]);
		} else {
			for (int i = 0; i < 4; i++) {
				verts[pos[1] + 4 + 5 * i] = button + 11;
			}
		}

	} else if (pos[0] != 0) {
		if (pos[1] == verts.size() - 20) {
			for (int i = 0; i < 20; i++) verts.pop_back();
		} else {
			verts.erase(verts.begin() + pos[1], verts.begin() + pos[1] + 20);
		}
		if (pos[0] == indices.size() - 6) {
			for (int i = 0; i < 6; i++) indices.pop_back();
		} else {
			indices.erase(indices.begin() + pos[0], indices.begin() + pos[0] + 6);
		}
		pos[0] = 0;
	}
	updateVAO();
}

void gameUI::setupVAO() {

	GLuint vs, fs, ebo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLint), verts.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

	GLuint tboMain, tboNums, tboBut;
	
	loadBMP("gui.png", tboMain);
	loadBMP("nums.png", tboNums);
	loadBMP("buttons.png", tboBut);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, tboMain);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, tboNums);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, tboBut);

	glActiveTexture(GL_TEXTURE0);

	vs = compileVShader("main_vs");
	fs = compileFShader("UI_fs");
	prog = createProgram(vs, fs);

	uniforms[0] = glGetUniformLocation(prog, "main_tex");
	uniforms[1] = glGetUniformLocation(prog, "num_tex");
	uniforms[2] = glGetUniformLocation(prog, "button_tex");

	GLint posAttrib = glGetAttribLocation(prog, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_INT, GL_FALSE, 5 * sizeof(GLint), 0);

	GLint texAttrib = glGetAttribLocation(prog, "tex_coord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 3, GL_INT, GL_FALSE, 5 * sizeof(GLint), (void*)(2 * sizeof(GLint)));

	glBindVertexArray(0);
}

void gameUI::mainOverlay() {

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

void gameUI::initText() {

	GLint textVerts[] {
		5, 62, 598,			//coins
		5, 68, 456,			//total pop
		4, 5, 173,			//total atk
		4, 77, 173,			//total def
		4, 75, 525,			//pop
		4, 5, 283,			//atk
		4, 77, 283,			//def
		3, 907, 565,		//send atk
		3, 963, 565,		//send def
		5, 248, 762,		//score
		3, 833, 762,		//turn
	};

	GLushort textIndices[] = {
			0, 1, 2,
			2, 3, 0,
	};

	GLushort baseIndex = 4;

	for (int i = 0; i < 33; i+=3) {		// 11 different locations * 3 = 33
		for (int j = 0; j < textVerts[i]; j++) {	// number of letters at location
			for (int k = 0; k < 2; k++) {	// 2x2 square
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

void gameUI::updateVAO() {
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLint), verts.data(), GL_STATIC_DRAW);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}

bool gameUI::changeText(Text type, int num) {

	static int vals[11];
	static int countDigits[11] = {	//to make sure the int isn't too many digits
		5, 5, 4, 4, 4, 4, 4, 3, 3, 5, 3
	};

	int typeNum = (int)type;

	if (typeNum < 4) {	//coins, totalPop, totalAtk, totalDef are not set explicitly
		vals[typeNum] += num;
	} else {
		vals[typeNum] = num;
	}

	int maxLength = countDigits[typeNum];
	if (num >= pow(10, maxLength)) return false;

	int *newNum = new int[maxLength];
	for (int i = maxLength - 1; i > -1; i--) {
		newNum[maxLength - (i + 1)] = (int)(vals[typeNum] / pow(10, i)) % 10;
	}

	int baseIndex = 24;

	int myBaseIndex = 0;

	for (int i = 0; i < typeNum; i++) {
		myBaseIndex += countDigits[i] * 20;
	}

	baseIndex += myBaseIndex;

	for (int i = 0; i < maxLength; i++) {
		for (int j = 0; j < 4; j++)
			verts[baseIndex + i * 20 + j * 5] = newNum[i];
	}

	delete[] newNum;
	updateVAO();
	return true;
}

const int gameUI::sectionCoords[] = {
	147, 32, 867, 768,		// map
	915, 292, 1000, 338,	// send troops
	902, 158, 950, 186,		// atk up
	902, 252, 950, 281,		// atk down
	959, 158, 1008, 186,	// def up
	959, 252, 1008, 281,	// def down
	900, 602, 1007, 653,	// buy attack
	900, 665, 1007, 720,	// buy defence
	900, 525, 1007, 590,	// buy farm
	900, 440, 1007, 515,	// buy bank
	0, 34, 137, 130,		// settings
	3, 2, 31, 22,			// close
};

gameUI::Section gameUI::pointInBox(int x, int y) {

	int tx, bx, ty, by;
	for (int i = 0; i < 48; i += 4) {
		tx = sectionCoords[i];
		ty = sectionCoords[i + 1];
		bx = sectionCoords[i + 2];
		by = sectionCoords[i + 3];
		if (x >= tx && x <= bx && y >= ty && y <= by) return (gameUI::Section)(i / 4);
	}
	return (gameUI::Section)(12);
}