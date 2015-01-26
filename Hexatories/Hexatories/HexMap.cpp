#include <fstream>
#include "HexMap.h"
#include "Shaders.h"
#include "log.h"

using namespace std;

void HexTile::initTile(int xc, int yc, GLfloat vboArray[], int pos, int type) {

	/*
		Default vertex array. These are adjusted for each hex based on its grid position.
	*/
	GLfloat verts[] = {
		152, 729,
		158, 718,
		170, 718,
		176, 729,
		170, 740,
		158, 740,
	};
	
	/*
		The colour values for each tile. RGB, but rather than 0-255 it's 0.0f - 1.0f.
		To convert RGB value to modulated value use the following formula:
		modVal = colVal / 255;
		Round to 3 d.p. for simplicity.
	*/
	GLfloat types[] = {
		0.722f, 0.584f, 0.243f,		// sand
		0.243f, 0.38f, 0.722f,		// water
		0.251f, 0.659f, 0.259f,		// grass
	};

	/*
		xoff - either 1 or 0 depending on your x value. Needed as every other column is lower down (look at the map if confused).
		colPos - the start position in the types array of your selected colour. e.g. if type was 1 (water) colPos would be 3.
	*/
	int xoff = xc % 2;
	int colPos = type * 3;

	/*
		Each vertex has 5 floats which define it. x, y, r, g, b.
		Goes through each of the vertices (of which there are 6, 6 * 5 = 30) defining their floats.
	
		i will always be a multiple of 5, for the 5 floats per vertex.
		pos is the first free position in the array (the end of the array).
		Therefore using pos + i + x allows you to define these floats individually and per vertex.

		e.g. 
		pos + i = x position of the current tile
		pos + i + 1 = y position of the current tile

		note: i / 2.5 will increase by 2 each loop, and is used for the x/y positions from the verts array.
		Cast is needed just for the compiler to not throw a fit.
	*/
	for (int i = 0; i < 30; i += 5) {

		vboArray[pos + i] = verts[(int)(i / 2.5)] + (18 * xc);	// Calculate the x pos of the current tile and store it.
		vboArray[pos + i + 1] = verts[(int)(i / 2.5) + 1] - (22 * yc + 11 * xoff); // Calculate the y pos and store it.

		for (int j = colPos; j < colPos + 3; j++) {	// Loops through the colour array, appending your selected colour to the end of each vertex.
			vboArray[pos + i + 2 + (j % 3)] = types[j];
		}
	}
}

HexMap::HexMap() {
	/*
		pos - start position in vertex array
		currTile - the current tile not in columns/rows. For instance the 3rd tile in the 2nd column would be (1 * 41 + 3). 41 being the column height.
		baseIndices - the draw order for the hex tile vertices. Needs to be specified for each tile, hence "base".
	*/
	int pos = 0;
	int currTile = 0;
	short baseIndices[] = { 0, 1, 5, 1, 2, 5, 2, 4, 5, 2, 3, 4 };

	string mapCode = mapFromFile("map");

	/*
		verts - the vertices for all tiles. Includes colour information.
		indices - the draw order for all vertices.
		myHex - just to use the initHex function, needs reordering but we'll see what we do with the class structures.
		vbo, ebo - self explanatory, can be freed at function end.
	*/
	GLfloat verts[39600];
	short indices[15840];
	HexTile myHex;
	GLuint vbo, ebo;

	/*
		Sets up the default map if the one loaded from file isn't correct in some way. Just fills a string with "map" and 2132 zeros.
	*/
	if (mapCode.empty()) {
		string defaultMap(2003, '0');
		mapCode.append("map");
		mapCode.append(defaultMap);
	}

	int tileType;

	/*
		Loops through each tile getting its vertex information.
	*/
	for (int x = 0; x < 40; x++) {
		for (int y = 0; y < 33; y++) {
			
			tileType = mapCode.at(3 + currTile) - '0';	// Gets the tile colour number.
			if (tileType > 2) tileType = 0;	// If it's invalid set to default.

			myHex.initTile(x, y, verts, pos, tileType);

			for (int i = 0; i < 12; i++) {	// Set up the vertex draw order for the current tile.
				indices[currTile * 12 + i] = baseIndices[i] + 6 * currTile;
			}

			currTile++;
			pos += 30;
		}
	}
	
	/*
		Set up the program for drawing the map.
	*/
	GLuint vs, fs;
	vs = compileVShader("mapColour_vs");
	fs = compileFShader("mapColour_fs");
	progCol = createProgram(vs, fs);

	/*
		Sets up all the vertex attribute stuff. In order of code blocks:

		Define and bind vao.
		Define and bind vbo. (Vertex information)
		Define and bind ebo. (Draw order)
		Define position variable for shader.
		Define colour variable for shader.
		Unbind vao.
	*/
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

	/*
		Sets up your filepath for you, because I'm nice like that.
	*/
	string filePath = "Maps\\";
	filePath.append(path);
	filePath.append(".mp");

	/*
		Opens your file for reading. Look up fstream if confused. Basically allows for simpler file handling.
	*/
	ifstream file(filePath);

	if (file.is_open()) {

		string line;
		getline(file, line);	// No loop needed, all maps are one long line (at least right now).
		mapString.append(line);
		
		file.close();

	} else {
		log("Failed to load file: %s\n", filePath.c_str());
		return mapString;
	}
	/*
		Ensure the first 3 chars are "map"
	*/
	if (mapString.compare(0, 3, "map")) {
		log("Not a valid map: %s\n", filePath.c_str());
		mapString.clear();
		return mapString;
	}
	return mapString;
}

bool pointToTile(double mouseX, double mouseY, int &gridX, int &gridY) {

	/*
	Offsets
	*/
	mouseX = mouseX - 152;
	mouseY = mouseY - 28;

	/*
	Bounds check
	*/
	if (mouseX < 0 || mouseX > 740 || mouseY > 740) return false;

	/*
	Find rectangle within which point lies. Each rect has sections of 3 different tiles in.
	*/
	int rectX = (int)mouseX / 18;
	int rectY = (int)(mouseY - ((rectX % 2) * 11)) / 22;

	/*
	Mouse position relative to the current box
	*/
	mouseX -= 18 * rectX;
	mouseY -= 22 * rectY + ((rectX % 2) * 11);

	/*
	Inequality, test if we are in the main tile of this rectangle or the two smaller sections.
	If we are, grid co ords are rect co ords, otherwise x is -1 and y is rect +1/0/-1
	*/
	if (mouseX > 12 * abs(0.5 - mouseY / 22)) {
		gridX = rectX;
		gridY = rectY;
	} else {
		gridX = rectX - 1;
		gridY = rectY - (gridX % 2) + ((mouseY > 11) ? 1 : 0);
	}

	/*
	Final bounds test
	*/
	if (gridX < 0 || gridX > 39 || gridY < 0 || gridY > 32) return false;

	std::printf("x: %d, y: %d\n", gridX, gridY);
	return true;
}