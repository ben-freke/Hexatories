#include "AI.h"

using namespace std;

void AI::initAI(vector<Territory> &all, player &me) {
	allTerrs = all;
	//me = me;
}

/*vector<Territory> AI::moveAI() {
	fillArrays();
	handleDefence();
	return allTerrs[0];
}*/

/*void AI::fillArrays() {

	owned.clear();
	enemyBorders.clear();
	borders.clear();

	for (unsigned int i = 0; i < allTerrs.size(); i++) {

		if (allTerrs[i].getOwner() == 1) {
			owned.push_back(allTerrs[i].getOwner());
			if (allTerrs[i].getEnemyBorders(allTerrs, enemyBorders))
				borders.push_back[i];
		}
	}
}*/

/*void AI::handleDefence() {
	
	vector<int> dangerousEnemies;

	for (unsigned int i = 0; i < borders.size(); i++) {
		dangerousEnemies = allTerrs[borders[i]].isVulnerable(allTerrs);
		if (!dangerousEnemies.empty()) {
			int maxAttack;
			int defense = allTerrs[borders[i]].getDefense();
			for (unsigned int j = 0; j < dangerousEnemies.size(); j++) {
				int atk = allTerrs[dangerousEnemies[j]].getAttackers() * 15;
				maxAttack += atk;
			}
		}
	}
}*/