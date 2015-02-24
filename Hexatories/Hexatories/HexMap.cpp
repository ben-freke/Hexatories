#include <iostream>
#include <fstream>
#include <algorithm>
#include "HexMap.h"
#include "Log.h"
#include "Shaders.h"
#include "Textures.h"

using namespace std;

void HexMap::initMap(vector<Territory> &ter, bool setupTer) {

	string mapPath;
	//cout << "Map name?\n";
	//cin >> mapPath;

	int *mapCode = HexMap::mapFromFile("map");
	while (mapCode == NULL) {
		printf("Invalid map\n");
		cin >> mapPath;
	}

	/*
		Clear openGL vectors (in case we are loading a new game)
	*/
	tileVerts.clear();
	indices.clear();

	int mapPos = getAllTiles(mapCode);	//loads all tile vertices

	addOverlay();	// adds the tile wireframe

	if (setupTer) {	//if we are setting up territories, initialise them and get their borders
		int terrCount = setupTerritories(mapCode, mapPos, ter);
		for (unsigned int i = 0; i < ter.size(); i++) {
			ter[i].getBorderVBO(tileVerts, indices);
		}
	}

	if (firstTime) {	//if we've never been initialised before, setup textures etc.
		setupVAO();
		firstTime = false;
	} else {
		updateVAO();
	}

}

void HexMap::drawMap() {

	glBindVertexArray(vaoMap);

	glUseProgram(progMap);

	for (int i = 0; i < 3; i++)
		glUniform1i(uniforms[i], i + 1);	//tell gpu which textures to use

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);
}

tile_t HexMap::pointToTile(double mouseX, double mouseY) {

	/*
	Offsets
	*/
	mouseX -= 147;
	mouseY -= 32;

	int gridX, gridY;

	tile_t nullTile = { -1, -1, -1 };

	if (mouseX > 720 || mouseY > 736 || mouseX < 0 || mouseY < 0) return nullTile;

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
	if (mouseX > 12 * abs(0.5 - ((mouseY < 0) ? mouseY * -1 : mouseY) / 22)) {
		if (mouseY < 0) return nullTile;
		gridX = rectX;
		gridY = rectY;
	} else {
		gridX = rectX - 1;
		gridY = rectY - (gridX % 2) + ((mouseY > 11) ? 1 : 0);
	}

	/*
		Final bounds test
	*/
	if (gridX < 0 || gridX > 39 || gridY < 0 || gridY > 32) return nullTile;

	return (*find_if(allTiles.begin(), allTiles.end(), findTile(gridX, gridY)));
}

void HexMap::addToTiles(tile_t newTile) {
	allTiles.push_back(newTile);
}

#pragma region openglStuff
void HexMap::setupVAO() {

	/*
	Vertex shader, fragment shader
	*/
	GLuint vs, fs, eboMap;

	/*
	Sets up all the vertex attribute stuff. In order of code blocks:

	Define and bind vao.
	Define and bind vbo. (Vertex information)
	Define and bind ebo. (Draw order)
	Load textures.
	Bind textures to slots.
	Load program.
	Get uniform variable positions in program
	Define position variable for shader.
	Define colour variable for shader.
	Define texture variable for shader.
	Unbind vao.
	*/
	glGenVertexArrays(1, &vaoMap);
	glBindVertexArray(vaoMap);

	glGenBuffers(1, &vboMap);
	glBindBuffer(GL_ARRAY_BUFFER, vboMap);
	glBufferData(GL_ARRAY_BUFFER, tileVerts.size() * sizeof(GLint), tileVerts.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &eboMap);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboMap);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

	GLuint tboBorder, tboMap, tboBuild;

	loadBMP("tileBorders.png", tboBorder);
	loadBMP("wireframe.png", tboMap);
	loadBMP("bankFarm.png", tboBuild);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tboBorder);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tboMap);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, tboBuild);

	glActiveTexture(GL_TEXTURE0);

	vs = compileVShader("map_vs");
	fs = compileFShader("map_fs");
	progMap = createProgram(vs, fs);

	uniforms[0] = glGetUniformLocation(progMap, "border_tex");
	uniforms[1] = glGetUniformLocation(progMap, "map_tex");
	uniforms[2] = glGetUniformLocation(progMap, "farm_bank_tex");

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

void HexMap::updateVAO() {
	glBindVertexArray(vaoMap);

	glBindBuffer(GL_ARRAY_BUFFER, vboMap);
	glBufferData(GL_ARRAY_BUFFER, tileVerts.size() * sizeof(GLint), tileVerts.data(), GL_STATIC_DRAW);	//update vbo

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);	//update ebo

	glBindVertexArray(0);
}

void HexMap::addOverlay() {

	/*
	The vertices of the corners of the map (for the wireframe overlay).
	Beneath is the indices for the draw order of the triangles that make up this rectangle.
	*/
	GLint tileVertsTex[] = {
		147, 736, 0, 0, 0, 0, 0, 4,
		873, 736, 0, 0, 0, 1, 0, 4,
		873, 0, 0, 0, 0, 1, 1, 4,
		147, 0, 0, 0, 0, 0, 1, 4,
	};

	GLushort rectIndices[] = {
		0, 1, 2,
		2, 3, 0,
	};

	/*
	Adds these vertices to the end of the vbo and the indices to the ebo.
	*/
	for (int i = 0; i < 6; i++) {
		int vertCount = tileVerts.size() / 8;
		indices.push_back(vertCount + rectIndices[i]);
	}

	for (int i = 0; i < 32; i++) {
		tileVerts.push_back(tileVertsTex[i]);
	}
}

int HexMap::getAllTiles(int *mapCode) {
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

#pragma region initTiles
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

			calcTileVerts(x, y, colType);

			for (int i = 0; i < 12; i++) {	// Set up the vertex draw order for the current tile.
				indices.push_back(baseIndices[i] + 6 * currTile);
			}

			currTile++;
		}
	}
	return mapPos;
}
#pragma endregion

#pragma region defaultTileVertsCols

const GLint HexMap::defTileVerts[] = {
	147, 725,	//left
	153, 714,	//bottom left
	165, 714,	//bottom right
	171, 725,	//right
	165, 736,	//top right
	153, 736,	//top left
};

const GLint HexMap::cols[] = {
	182, 149, 62,	// sand
	23, 49, 122,	// water
	64, 168, 66,	// grass
};
#pragma endregion

void HexMap::calcTileVerts(int x, int y, int col) {

	/*
	xoff - either 1 or 0 depending on your x value. Needed as every other column is lower down (look at the map if confused).
	colPos - the start position in the types array of your selected colour. e.g. if type was 1 (water) colPos would be 3.
	*/
	int xoff = x % 2;
	int colPos = col * 3;

	/*
	Each vertex has 8 floats which define it. x, y, r, g, b, texX, texY, texZ.
	Goes through each of the vertices (of which there are 6) defining their floats.

	i will always be a multiple of 2, for the x, y floats per vertex. 

	note: i will increase by 2 each loop, and is used for the x/y positions from the tileVerts array.
	Cast is needed just for the compiler to not throw a fit.
	*/
	for (int i = 0; i < 12; i += 2) {

		tileVerts.push_back(defTileVerts[(int)i] + (18 * x));	// Calculate the x pos of the current tile and store it.
		tileVerts.push_back(defTileVerts[(int)i + 1] - (22 * y + 11 * xoff));	// Calculate the y pos and store it.

		for (int j = colPos; j < colPos + 3; j++) {		// Loops through the colour array, appending your selected colour to the end of each vertex.
			tileVerts.push_back(cols[j]);
		}

		/*
		Invalid texture coords so the shader knows what's up
		*/
		for (int j = 0; j < 3; j++) {
			tileVerts.push_back(-1);
		}
	}
}
#pragma endregion

void HexMap::updateBorder(Territory &ter, bool load) {
	if (load) {	//if we are being called from a load game
		ter.getBorderVBO(tileVerts, indices);	//add border details to the vbo
	} else {	//otherwise
		ter.updateBorderVBO(tileVerts);	//update the border details (which will have already been added)
	}
	updateVAO();
}

void HexMap::updateBuilding(Territory *ter, bool building) {
	if (ter == NULL) return;
	ter->addBuilding(building, tileVerts, indices);	// adds building to the terr, true = farm, false = bank
	updateVAO();
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

	tile_t currTile;

	/*
	Loops through each territory, finds how many tiles in each and loops through each of those. Adds all tiles to an array and then passes that to the territory init.
	*/
	for (int i = 0; i < terrCount; i++) {
		

		ter.push_back(territory);
		int owner = mapCode[mapPos++];
		tilesInTerr = mapCode[mapPos++];

		for (int j = 0; j < tilesInTerr; j++) {

			tileNum = mapCode[mapPos++];
			currTile = { tileNum / 33, tileNum % 33, i };
			tiles.push_back(currTile);
			allTiles.push_back(currTile);
		}
		
		ter[i].initTerritory(tiles, tilesInTerr, owner, i);
		tiles.clear();
	}

	delete[] mapCode;
	return terrCount;
}

int *HexMap::mapFromFile(string path) {
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