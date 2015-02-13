#include <vector>
#include <GL\glew.h>
#include <GLFW/glfw3.h>

#ifndef __TERRITORY_H
#define __TERRITORY_H

class Territory {

	std::vector<int> tiles, border;
	int size;

public:

	void initTerritory(std::vector<int>, int);

	void setupBorderTiles();

	void getBorderVBO();

};
#endif