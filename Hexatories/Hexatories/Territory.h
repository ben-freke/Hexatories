#include <vector>
#include <GL\glew.h>
#include <GLFW/glfw3.h>

#ifndef __TERRITORY_H
#define __TERRITORY_H

class Territory {

	std::vector<int> tiles, border;
	int size;
	int populationNo, troopsAttack, troopsDefense, territoryID, owner = 0;
	bool farmBuilt, bankBuilt = false;

public:

	void initTerritory(std::vector<int>, int);

	int getAttackScore();

	void setAttackScore(int value);

	int getDefenseScore();

	void setDefenseScore(int value);

	void setupBorderTiles();

	void getBorderVBO();

};
#endif