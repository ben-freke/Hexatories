#include <fstream>
#include "HexMap.h"
#include "Log.h"
#include "Shaders.h"
#include "Textures.h"

using namespace std;

bool HexMap::initMap(vector<Territory> &ter) {

	int *mapCode = HexMap::mapFromFile("map");
	if (mapCode == NULL) {
		log("Invalid map\n");
		return false;
	}

	vector<GLint> tileVerts;
	vector<GLushort> indices;

	int mapPos = getAllTiles(mapCode, tileVerts, indices);
	addOverlay(tileVerts, indices);

	int terrCount = setupTerritories(mapCode, mapPos, ter);
	
	for (int i = 0; i < terrCount; i++) {
		ter[i].getBorderVBO(tileVerts, indices);
	}

	setupVAO(tileVerts, indices);

	return true;
}

int HexMap::setupTerritories(int *mapCode, int mapPos, vector<Territory> &ter) {
	/*
	Gets the total amount of territories and requests that much space from memory.
	*/
	int terrCount = mapCode[mapPos++];

	vector<tile_t> tiles;
	int tileNum;
	int tilesInTerr;
	Territory territory;
	/*
	Loops through each territory, finds how many tiles in each and loops through each of those. Adds all tiles to an array and then passes that to the territory init.
	*/
	for (int i = 0; i < terrCount; i++) {

		ter.push_back(territory);
		int owner = mapCode[mapPos++];
		tilesInTerr = mapCode[mapPos++];

		for (int j = 0; j < tilesInTerr; j++) {

			tileNum = mapCode[mapPos++];
			tiles.push_back({ tileNum / 33, tileNum % 33 });
		}
		
		ter[i].initTerritory(tiles, tilesInTerr, owner);
		tiles.clear();
	}

	delete[] mapCode;
	return terrCount;
}

void HexMap::setupVAO(vector<GLint> verts, vector<GLushort> indices) {

	/*
	Vertex shader, fragment shader
	*/
	GLuint vs, fs, vboMap, eboMap;

	/*
	Sets up all the vertex attribute stuff. In order of code blocks:

	Define and bind vao.
	Define and bind vbo. (Vertex information)
	Define and bind ebo. (Draw order)
	Load texture.
	Load program.
	Define position variable for shader.
	Define colour variable for shader.
	Define texture variable for shader.
	Unbind vao.
	*/
	glGenVertexArrays(1, &vaoMap);
	glBindVertexArray(vaoMap);

	glGenBuffers(1, &vboMap);
	glBindBuffer(GL_ARRAY_BUFFER, vboMap);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLint), verts.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &eboMap);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboMap);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

	numIndices = indices.size();

	GLuint tboBorder, tboMap;

	loadBMP("tileBorders.png", tboBorder);
	loadBMP("wireframe.png", tboMap);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tboBorder);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tboMap);

	glActiveTexture(GL_TEXTURE0);

	vs = compileVShader("map_vs");
	fs = compileFShader("map_fs");
	progMap = createProgram(vs, fs);
	
	uniforms[0] = glGetUniformLocation(progMap, "border_tex");
	uniforms[1] = glGetUniformLocation(progMap, "map_tex");

	GLint posAttrib = glGetAttribLocation(progMap, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_INT, GL_FALSE, 8 * sizeof(GLint), 0);

	GLint colAttrib = glGetAttribLocation(progMap, "colour");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_INT, GL_FALSE, 8 * sizeof(GLint), (void*)(2 * sizeof(GLint)));

	GLint texAttrib = glGetAttribLocation(progMap, "tex_coord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 3, GL_INT, GL_FALSE, 8 * sizeof(GLint), (void*)(5 * sizeof(GLint)));

	glBindVertexArray(0);
}

void HexMap::addOverlay(vector<GLint> &verts, vector<GLushort> &indices) {

	/*
	The vertices of the corners of the map (for the wireframe overlay).
	Beneath is the indices for the draw order of the triangles that make up this rectangle.
	*/
	GLint vertsTex[] = {
		152, 740, 0, 0, 0, 0, 0, 3, 
		878, 740, 0, 0, 0, 1, 0, 3,
		878, 0, 0, 0, 0, 1, 1, 3,
		152, 0, 0, 0, 0, 0, 1, 3,
	};

	GLushort rectIndices[] = {
		0, 1, 2,
		2, 3, 0,
	};

	/*
	Adds these vertices to the end of the vbo and the indices to the ebo.
	*/
	for (int i = 0; i < 6; i++) {
		int vertCount = verts.size() / 8;
		indices.push_back(vertCount + rectIndices[i]);
	}

	for (int i = 0; i < 32; i++) {
		verts.push_back(vertsTex[i]);
	}
}

int HexMap::getAllTiles(int *mapCode, vector<GLint> &verts, vector<GLushort> &indices) {
#pragma region vars
	/*
	currTile - the current tile not in columns/rows. For instance the 3rd tile in the 2nd column would be (1 * 33 + 3). 41 being the column height.
	mapPos - how far through the map data we are (in terms of array elements)
	colCount - to count how many tiles to colour each time.
	colType - the current colour.
	baseIndices - the draw order for the hex tile vertices. Needs to be specified for each tile, hence "base".
	*/
	int currTile = 0;
	int mapPos = 2;
	int colCount = mapCode[0];
	int colType = mapCode[1];

	GLushort baseIndices[] = { 0, 1, 5, 1, 2, 5, 2, 4, 5, 2, 3, 4 };

#pragma endregion

#pragma region initTilesTerrs
	/*
	Loops through each tile generating its vertex information and initialising them.
	*/
	for (int x = 0; x < 40; x++) {
		for (int y = 0; y < 33; y++) {

			/*
			Goes through colCount tiles making each of them colour colType, when this is true the colCount is incremented and colType changed.
			*/
			while (currTile == colCount) {
				colCount += mapCode[mapPos++];
				colType = mapCode[mapPos++];
			}

			HexMap::calcTileVerts(x, y, colType, verts);

			for (int i = 0; i < 12; i++) {	// Set up the vertex draw order for the current tile.
				indices.push_back(baseIndices[i] + 6 * currTile);
			}

			currTile++;
		}
	}
	return mapPos;
}

void HexMap::drawMap() {

	glBindVertexArray(vaoMap);

	glUseProgram(progMap);

	for (int i = 0; i < 2; i++)
		glUniform1i(uniforms[i], i);

	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);
}

#pragma region defaultVertsCols

const GLint HexMap::defTileVerts[] = {
	152, 729,	//left
	158, 718,	//bottom left
	170, 718,	//bottom right
	176, 729,	//right
	170, 740,	//top right
	158, 740,	//top left
};

const GLint HexMap::cols[] = {
	182, 149, 62,	// sand
	62, 97, 182,	// water
	64, 168, 66,	// grass
};
#pragma endregion

void HexMap::calcTileVerts(int x, int y, int col, vector<GLint> &vboArray) {

	/*
	xoff - either 1 or 0 depending on your x value. Needed as every other column is lower down (look at the map if confused).
	colPos - the start position in the types array of your selected colour. e.g. if type was 1 (water) colPos would be 3.
	*/
	int xoff = x % 2;
	int colPos = col * 3;

	/*
	Each vertex has 7 floats which define it. x, y, r, g, b, texX, texY.
	Goes through each of the vertices (of which there are 6, 6 * 7 = 42) defining their floats.

	i will always be a multiple of 7, for the 7 floats per vertex.

	note: i / 3.5 will increase by 2 each loop, and is used for the x/y positions from the verts array.
	Cast is needed just for the compiler to not throw a fit.
	*/
	for (int i = 0; i < 42; i += 7) {

		vboArray.push_back(defTileVerts[(int)(i / 3.5)] + (18 * x));	// Calculate the x pos of the current tile and store it.
		vboArray.push_back(defTileVerts[(int)(i / 3.5) + 1] - (22 * y + 11 * xoff));	// Calculate the y pos and store it.

		for (int j = colPos; j < colPos + 3; j++) {		// Loops through the colour array, appending your selected colour to the end of each vertex.
			vboArray.push_back(cols[j]);
		}

		/*
		Invalid texture coords so the shader knows what's up
		*/
		for (int j = 0; j < 3; j++) {
			vboArray.push_back(-1);
		}
	}
}

int *HexMap::mapFromFile(const char *path) {
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
		return NULL;
	}
	/*
	Ensure the first 3 chars are "map"
	*/
	if (mapString.compare(0, 3, "map")) {
		log("Not a valid map: %s\n", filePath.c_str());
		mapString.clear();
		return NULL;
	}

	mapString.erase(0, 3);	// Remove "map"

	vector<string> subStrings;
	int delPos;

	while (!mapString.empty()) {

		delPos = mapString.find(",");
		subStrings.push_back(mapString.substr(0, delPos));
		mapString.erase(0, delPos + 1);

	}

	int *mapCode = new int[subStrings.size()];

	for (unsigned int i = 0; i < subStrings.size(); i++) {
		mapCode[i] = atoi(subStrings[i].c_str());
	}

	return mapCode;
}

bool HexMap::pointToTile(double mouseX, double mouseY, int &gridX, int &gridY) {

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