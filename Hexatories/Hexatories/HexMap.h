#include <vector>
#include <string>
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "Territory.h"

#ifndef __HEXMAP_H
#define __HEXMAP_H

class HexMap {
#pragma region vars
	bool firstTime = true;	//first time initialised?

	std::vector<tile_t> allTiles;	//all the hextiles (to check which terr they are in)
#pragma endregion

#pragma region openglStuff
	/*
	Default vertex array. These are adjusted for each hex based on its grid position.
	*/
	static const GLint defTileVerts[];

	/*
		All vertices and all indices
	*/
	std::vector<GLint> tileVerts;
	std::vector<GLushort> indices;

	/*
	The colour values for each tile. RGB.
	*/
	static const GLint cols[];

	GLint uniforms[3];	//for opengl textures

	GLuint vaoMap, vboMap, progMap;	//vao, vbo, program

	/*
	Sets up our VAO and loads all textures needed
	*/
	void setupVAO();

	/*
	Updates our VAO with new VBO/EBO
	*/
	void updateVAO();

	/*
	Adds the wireframe hexagons to screen
	*/
	void addOverlay();

	/*
		Calculates all tile vertices and adds to the tileVerts
	*/
	int getAllTiles(int *);

	/*
		Calculate vertex position. Also includes the colour of each vertex.
		Usage:
		HexTile.initTile(xPos, yPos, col);
		It appends the tiles vertices to the end of the verts array
	*/
	void calcTileVerts(int, int, int);
#pragma endregion

	/*
		Sets up all territories from the passed map
	*/
	int setupTerritories(int *, int, std::vector<Territory> &);

	/*
		Returns the string containing map data to interpret.
		Usage: int *mapCode = mapFromFile(mapPath);
	*/
	static int *mapFromFile(std::string);

public:

	/*
		Initialise the map, if bool is true initialise territories
	*/
	void initMap(std::vector<Territory> &, bool);

	/*
		Draws all map stuff
	*/
	void drawMap();

	/*
		Converts mouse point to tile in the map.
		Return null tile if invalid
	*/
	tile_t pointToTile(double, double);

	/*
		Adds a tile to allTiles to keep track of all tiles & there territories
	*/
	void addToTiles(std::vector<tile_t>);

	/*
		Updates the VBO from the territory, if true it will add to the vbo, false just update
	*/
	void updateBorder(Territory &, bool);

	/*
		Adds a building to the map (and territory) true = farm, false = bank
	*/
	void updateBuilding(Territory *, bool);
};
#endif