#include <vector>
#include "Territory.h"

#ifndef __AI_H
#define __AI_H

/*
Contains necessary information to remember about a player
*/
struct player {
	int coins, score, population;
};

class AI {

	std::vector<Territory> allTerrs;
	std::vector<int> owned, enemyBorders, borders;

	player me;

	void fillArrays();
	void handleDefence();

public:
	void initAI(std::vector<Territory> &, player &);
	std::vector<Territory> moveAI();
};

#endif
