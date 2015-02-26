#include "MainMenu.h"
#include "Textures.h"
#include "Shaders.h"

using namespace std;

void MainMenu::initMenu(int screen) {
	if (verts.empty()) {
		setupVAO();
	} else {
		changeScreen(screen);
	}
}

void MainMenu::changeScreen(int screen) {
	for (int i = 4; i < 20; i += 5) 
		verts[i] = screen;
	updateVAO();
}

void MainMenu::drawMenu() {
	glBindVertexArray(vao);

	glUseProgram(prog);

	glUniform1i(uniform, 10);	//Tell gpu which textures to use

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);
}

void MainMenu::setupVAO() {

	GLint rectVerts[] = {
		0, 768, 0, 0, 0,
		1024, 768, 1, 0, 0,
		1024, 0, 1, 1, 0,
		0, 0, 0, 1, 0,
	};

	GLushort rectIndices[] {
		0, 1, 2,
		2, 3, 0,
	};

	for (int i = 0; i < 20; i++)
		verts.push_back(rectVerts[i]);

	GLuint vs, fs, ebo;

	glGenVertexArrays(1, &vao);	//VAO contains all information for drawing the UI, including VBO & EBO
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);	//Contains all verts information
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLint), verts.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);	//Contains draw order of verts
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLushort), rectIndices, GL_STATIC_DRAW);

	GLuint tboMain;	//for our textures

	loadBMP("mainmenu.png", tboMain);

	glActiveTexture(GL_TEXTURE10);	//bind our textures to various slots
	glBindTexture(GL_TEXTURE_2D, tboMain);

	glActiveTexture(GL_TEXTURE0);	//reset bound slot

	vs = compileVShader("main_vs");	//compile shader program
	fs = compileFShader("MM_fs");
	prog = createProgram(vs, fs);

	uniform = glGetUniformLocation(prog, "main_tex");	//get position of these variables in our program and save them

	GLint posAttrib = glGetAttribLocation(prog, "position");	//set up our vertex attributes. (px, py, tx, ty, tz)
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_INT, GL_FALSE, 5 * sizeof(GLint), 0);

	GLint texAttrib = glGetAttribLocation(prog, "tex_coord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 3, GL_INT, GL_FALSE, 5 * sizeof(GLint), (void*)(2 * sizeof(GLint)));

	glBindVertexArray(0);

}

void MainMenu::updateVAO() {
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);	//update VBO on GPU
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLint), verts.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}

#pragma region sections
const int MainMenu::sectionCoords[] = {
	178, 529, 385, 594,		// tutorial
	401, 529, 627, 595,		// new game
	646, 529, 870, 594,		// load game
	18, 682, 217, 749,		// exit
};

MainMenu::Section MainMenu::pointInBox(int x, int y) {

	int tx, bx, ty, by;	// top x, bottom x, top y, bottom y

	for (int i = 0; i < 16; i += 4) {

		tx = sectionCoords[i];
		ty = sectionCoords[i + 1];
		bx = sectionCoords[i + 2];
		by = sectionCoords[i + 3];
		if (x >= tx && x <= bx && y >= ty && y <= by) return (MainMenu::Section)(i / 4); // just a point in box check

	}
	return (MainMenu::Section)(4);
}
#pragma endregion