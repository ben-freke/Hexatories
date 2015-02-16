#include <vector>
#include <string>
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "Territory.h"

#ifndef __HEXMAP_H
#define __HEXMAP_H

class HexMap {

	/*
		Default vertex array. These are adjusted for each hex based on its grid position.
	*/
	static const GLint defTileVerts[];
	std::vector<GLint> tileVerts;

	/*
		The colour values for each tile. RGB.
	*/
	static const GLint cols[];

	GLint uniforms[2];

	GLuint vaoMap, vboMap, progMap;
	unsigned int numIndices;

	std::vector<tile_t> allTiles;

	void updateVAO();

	int setupTerritories(int *, int, std::vector<Territory> &);

	void setupVAO(std::vector<GLushort>);

	void addOverlay(std::vector<GLushort> &);

	int getAllTiles(int *, std::vector<GLushort> &);

	/*
	Calculate vertex position. Also includes the colour of each vertex.
	Usage:
	HexTile.initTile(xPos, yPos, col, vertsArray);
	It appends the tiles vertices to the end of the passed array.
	*/
	void calcTileVerts(int, int, int);

	/*
	Returns the string containing map data to interpret. Really needs reworking.
	Usage: string map = mapFromFile("BeachMap");
	*/
	static int *mapFromFile(const char *);

public:

	bool initMap(std::vector<Territory> &);

	/*
	Just draws the map & grid currently.
	*/
	void drawMap();

	/*
	Converts mouse point to x and y of tile in the map.
	Return true if in the grid.
	*/
	tile_t pointToTile(double, double);

	void updateVBO(Territory);
};
#endif