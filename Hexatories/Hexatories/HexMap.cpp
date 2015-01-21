#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "HexMap.h"

void HexTile::initTile(int xc, int yc) {

	x = xc;
	y = yc;

	glGenVertexArrays(1, &vaoid);
	glBindVertexArray(vaoid);

	GLfloat verts[] = {
		-0.98125f, 0.964375f,		// left
		-0.96875f, 0.940625f,		// bottom left
		-0.94375f, 0.940625f,		// bottom right
		-0.93125f, 0.964375f,		// right
		-0.94375f, 0.988125f,		// top right
		-0.96875f, 0.988125f,		// top left
	};

	int xoff = xc % 2;

	for (int i = 0; i < 14; i += 2) {
		verts[i] += 0.0375f * xc;
		verts[i + 1] -= 0.0475f * yc + 0.02375f * xoff;
	}

	glGenBuffers(1, &vboid);
	glBindBuffer(GL_ARRAY_BUFFER, vboid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
}

void HexTile::drawTile() {
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vboid);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	short indices[] = { 0, 1, 2, 3, 4, 5 };

	glDrawElements(GL_POLYGON, 6, GL_UNSIGNED_SHORT, indices);

	glDisableVertexAttribArray(0);
}

HexMap::HexMap() {
	for (int x = 0; x < 52; x++) {
		for (int y = 0; y < 41; y++) {
			myHex[x][y].initTile(x, y);
		}
	}
}

void HexMap::drawMap() {
	for (int x = 0; x < 52; x++) {
		for (int y = 0; y < 41; y++) {
			myHex[x][y].drawTile();
		}
	}
}