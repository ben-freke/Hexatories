#include <vector>
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "Territory.h"
#include "HexMap.h"

#ifndef __GAME_H
#define __GAME_H

/*
	Currently contains functionality to generate the entire vbo of the hexmap and draw it.
	Also allows user designed maps to be read from file.
*/
class Game {

	int score, gold;

	HexMap map;
	std::vector<Territory> territories;

public:

	/*
		Initialises the map
	*/
	bool initGame();

	Territory getTerritory(double x, double y);

	/*
		Highlight territory
	*/
	void highlightTerritory(double, double);

	void changeTerritoryColour(Territory ter, int col);

	/*
		Batch draw function
	*/
	void draw();

};

#endif