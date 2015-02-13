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

	HexMap map;

public:
	/*
		Initialises the map
	*/
	bool initGame(Territory *);

	/*
		Batch draw function
	*/
	void draw();

};

#endif