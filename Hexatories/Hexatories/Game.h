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


public:
	HexMap map;
	std::vector<Territory> territories;

	/*
		Initialises the map
	*/
	bool initGame();

	/*
		Highlight territory
	*/
	void highlightTerritory(double, double);

	/*
		Batch draw function
	*/
	void draw();

};

#endif