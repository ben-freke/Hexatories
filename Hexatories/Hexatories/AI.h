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

	std::vector<Territory> *allTerrs;
	std::vector<int> toUpdateTerrNo, *farmArray, *bankArray;
	std::vector<TerrInfo> ourTerrs, ourVuln, theirVuln, ourSafe, theirSafe, enemyBorders, borders, inners;

	player *ai;

	int banks, farms;

	void fillArrays();
	void findReinforceTerrs(int &, std::vector<int> &, TerrInfo &, std::vector<TerrInfo> &, bool);
	void findAttackTerrs(int &, std::vector<int> &, TerrInfo &, std::vector<TerrInfo> &, std::vector<TerrInfo> &);
	void attack(std::vector<TerrInfo> &);
	void handleDefence();
	void handleAttack();
	void addBuildings();

public:
	void initAI(std::vector<Territory> *, player *);
	std::vector<int> moveAI(std::vector<int> *, std::vector<int> *);
};

#endif