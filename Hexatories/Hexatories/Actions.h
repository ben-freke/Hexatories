#ifndef __ACTIONS_H
#define __ACTIONS_H
#include "Territory.h"
#include <thread> 
#include <chrono> 
#include "Audio.h"
#include "HexMap.h"
#include "Game.h"

class Actions {
	int turnNo;
	bool attackFlag, moveFlag;
public:
	
	bool testClearPath(Territory territoryX, Territory territoryY);
	void increaseTurn();
	bool attack(Territory &territoryX, Territory &territoryY, Game &game);
};

#endif