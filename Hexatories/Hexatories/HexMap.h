#include <vector>
#include <string>
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "Territory.h"

#ifndef __HEXMAP_H
#define __HEXMAP_H

class HexMap {

	GLuint vaoMap, progMap;
	/*
		Default vertex array. These are adjusted for each hex based on its grid position.
	*/
	static const GLint defTileVerts[];

	/*
		The colour values for each tile. RGB.
	*/
	static const GLint cols[];

	GLint uniforms[2];

	unsigned int numIndices;

public:

	bool initMap(std::vector<Territory> &);

	int setupTerritories(int *, int, std::vector<Territory> &);

	void setupVAO(std::vector<GLint>, std::vector<GLushort>);

	void addOverlay(std::vector<GLint> &, std::vector<GLushort> &);

	int getAllTiles(int *, std::vector<GLint> &, std::vector<GLushort> &);

	/*
	Just draws the map & grid currently.
	*/
	void drawMap();

	/*
	Calculate vertex position. Also includes the colour of each vertex.
	Usage:
	HexTile.initTile(xPos, yPos, col, vertsArray);
	It appends the tiles vertices to the end of the passed array.
	*/
	static void calcTileVerts(int, int, int, std::vector<GLint> &);

	/*
	Returns the string containing map data to interpret. Really needs reworking.
	Usage: string map = mapFromFile("BeachMap");
	*/
	static int *mapFromFile(const char *);

	/*
	Converts mouse point to x and y of tile in the map.
	Return true if in the grid.
	*/
	static bool pointToTile(double, double, int &, int &);
};
#endif