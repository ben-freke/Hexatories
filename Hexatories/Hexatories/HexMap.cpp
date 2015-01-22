#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include "HexMap.h"
#include "Shaders.h"
#include "log.h"

using namespace std;

void HexTile::initTile(int xc, int yc, GLfloat vboArray[], int pos, int type) {

	GLfloat verts[] = {
		-0.98125f, 0.964375f,		// left
		-0.96875f, 0.940625f,		// bottom left
		-0.94375f, 0.940625f,		// bottom right
		-0.93125f, 0.964375f,		// right
		-0.94375f, 0.988125f,		// top right
		-0.96875f, 0.988125f,		// top left
	};

	GLfloat types[] = {
		0.722f, 0.584f, 0.243f,		// sand
		0.243f, 0.38f, 0.722f,		// water
		0.251f, 0.659f, 0.259f,		// grass
	};

	int xoff = xc % 2;
	int jType = type * 3;
	int j;

	for (int i = 0; i < 30; i += 5) {

		vboArray[pos + i] = verts[(int)(i / 2.5)] + (0.0375f * xc);
		vboArray[pos + i + 1] = verts[(int)(i / 2.5) + 1] - (0.0475f * yc + 0.02375f * xoff);

		for (j = jType; j < jType + 3; j++) {
			vboArray[pos + i + 2 + (j % 3)] = types[j];
		}
	}
}

HexMap::HexMap() {
	int pos = 0;

	short baseIndices[] = { 0, 1, 5, 1, 2, 5, 2, 4, 5, 2, 3, 4 };
	int currTile = 0;

	string mapCode = mapFromFile("map");
	if (mapCode.empty()) {
		string defaultMap(2132, '0');
		mapCode.append("map");
		mapCode.append(defaultMap);
	}

	int tileType;

	for (int x = 0; x < 52; x++) {
		for (int y = 0; y < 41; y++) {
			tileType = mapCode.at(3 + currTile) - '0';
			if (tileType > 2) tileType = 0;
			myHex.initTile(x, y, verts, pos, tileType);

			for (int i = 0; i < 12; i++) {
				indices[currTile * 12 + i] = baseIndices[i] + 6 * currTile;
			}

			currTile++;
			pos += 30;
		}
	}

	GLuint vs, fs;
	vs = compileVShader("mapColour_vs");
	fs = compileFShader("mapColour_fs");
	progCol = createProgram(vs, fs);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	GLint posAttrib = glGetAttribLocation(progCol, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	
	GLint colAttrib = glGetAttribLocation(progCol, "colour");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
	glBindVertexArray(0);
}

void HexMap::drawMap() {
	glBindVertexArray(vao);
	glUseProgram(progCol);
	glDrawElements(GL_TRIANGLES, 63960, GL_UNSIGNED_SHORT, 0);
}

string HexMap::mapFromFile(const char *path) {
	string mapString;

	string filePath = "Maps\\";
	filePath.append(path);
	filePath.append(".mp");

	ifstream file(filePath);

	if (file.is_open()) {

		string line;
		while (getline(file, line)) {
			mapString = mapString + line + "\n";
		}

		file.close();

	} else {
		log("Failed to load file: %s\n", filePath.c_str());
		return mapString;
	}

	if (mapString.compare(0, 3, "map")) {
		log("Not a valid map: %s\n", filePath.c_str());
		return mapString;
	}
	return mapString;
}