#include <vector>
#include <string>
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "Territory.h"

#ifndef __HEXMAP_H
#define __HEXMAP_H

class HexMap {
#pragma region vars
	/*
		Default vertex array. These are adjusted for each hex based on its grid position.
	*/
	static const GLint defTileVerts[];


	std::vector<GLint> tileVerts;
	std::vector<GLushort> indices;

	/*
		The colour values for each tile. RGB.
	*/
	static const GLint cols[];

	GLint uniforms[3];

	GLuint vaoMap, vboMap, progMap;

	bool firstTime = true;

	std::vector<tile_t> allTiles;
#pragma endregion

#pragma region openglStuff
	void setupVAO();
	void updateVAO();

	void addOverlay();

	int getAllTiles(int *);

	/*
	Calculate vertex position. Also includes the colour of each vertex.
	Usage:
	HexTile.initTile(xPos, yPos, col, vertsArray);
	It appends the tiles vertices to the end of the passed array.
	*/
	void calcTileVerts(int, int, int);
#pragma endregion

	int setupTerritories(int *, int, std::vector<Territory> &);

	/*
		Returns the string containing map data to interpret.
		Usage: string map = mapFromFile("BeachMap");
	*/
	static int *mapFromFile(std::string);

public:

	void initMap(std::vector<Territory> &, bool);

	/*
	Just draws the map & grid currently.
	*/
	void drawMap();

	/*
	Converts mouse point to x and y of tile in the map.
	Return true if in the grid.
	*/
	tile_t pointToTile(double, double);
	void addToTiles(tile_t);

	void updateBorder(Territory &, bool);
	void updateBuilding(Territory *, bool);
};
#endif