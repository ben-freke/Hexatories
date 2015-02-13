#ifndef __ACTIONS_H
#define __ACTIONS_H
#include "Territory.h"
#include <thread> 
#include <chrono> 
#include "Audio.h"
class Actions {
public:
	int turnNo;
	void attackTerritory(Territory territoryX, Territory territoryY, int owner);
	bool testClearPath(Territory territoryX, Territory territoryY);
	void increaseTurn();
	bool attack(Territory territoryX, Territory territoryY, int owner);
};

#endif