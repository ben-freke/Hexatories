#include "AI.h"
using namespace std;

void AI::initAI(vector<Territory> *all, player *me) {
	allTerrs = all;
	ai = me;
}

vector<int> AI::moveAI(vector<int> *farmsToBuild, vector<int> *banksToBuild) {

	farmArray = farmsToBuild;
	bankArray = banksToBuild;

	fillArrays();
	handleDefence();
	handleAttack();
	addBuildings();
	return toUpdateTerrNo;
	
}

void AI::fillArrays() {

	ourTerrs.clear();
	enemyBorders.clear();
	ourVuln.clear();
	theirVuln.clear();
	ourSafe.clear();
	borders.clear();
	inners.clear();
	farms = 0;
	banks = 0;

	vector<int> enemyBorderNums;

	for (unsigned int i = 0; i < (*allTerrs).size(); i++) {	//Get all our terrs
		if ((*allTerrs)[i].getOwner() == 1) {
			ourTerrs.push_back((*allTerrs)[i].getAIData((*allTerrs)));
		}
	}

	for (unsigned int i = 0; i < ourTerrs.size(); i++) {

		for (unsigned int j = 0; j < ourTerrs[i].enemyBorders.size(); j++) {	//Get all our enemy borders
			int terrNum = ourTerrs[i].enemyBorders[j];
			if (find(enemyBorderNums.begin(), enemyBorderNums.end(), terrNum) == enemyBorderNums.end()) {
				enemyBorders.push_back((*allTerrs)[terrNum].getAIData((*allTerrs)));
				enemyBorderNums.push_back(terrNum);
			}
		}

		if (ourTerrs[i].enemyBorders.size() > 0) {
			borders.push_back(ourTerrs[i]);
		} else {
			inners.push_back(ourTerrs[i]);
			if (!ourTerrs[i].bank) banks++;
			if (!ourTerrs[i].farm) farms++;
		}

		if (ourTerrs[i].maxAtk >= ourTerrs[i].defence) {	//Find which of our territories are vulnerable & hopefully safe
			ourVuln.push_back(ourTerrs[i]);
			ourTerrs.erase(ourTerrs.begin() + i--);
		} else if (ourTerrs[i].maxAtk < (int)(ourTerrs[i].defence - 75 * ourTerrs[i].enemyBorders.size())) {
			ourSafe.push_back(ourTerrs[i]);
			ourTerrs.erase(ourTerrs.begin() + i--);
		}
	}

	for (unsigned int j = 0; j < enemyBorders.size(); j++) {	//Find which enemy borders are vulnerable and which are safe

		if (enemyBorders[j].maxAtk >= enemyBorders[j].defence) {
			theirVuln.push_back(enemyBorders[j]);
			enemyBorders.erase(enemyBorders.begin() + j--);

		} else if (enemyBorders[j].maxAtk < (int)(enemyBorders[j].defence - 75 * enemyBorders[j].enemyBorders.size())) {
			theirSafe.push_back(enemyBorders[j]);
			enemyBorders.erase(enemyBorders.begin() + j--);
		}
	}
}

void AI::findReinforceTerrs(int &difference, vector<int> &reinforcingTerrs, TerrInfo &toReinforce, vector<TerrInfo> &searchSelection, bool troopType) {
	for (unsigned int j = 0; j < searchSelection.size(); j++) {	

		if (find_if(toReinforce.friendlyBorders.begin(), toReinforce.friendlyBorders.end(), findTerr(searchSelection[j].terrNo)) !=
			toReinforce.friendlyBorders.end()) {	
			int troop;

			if (!troopType)
				troop = (*allTerrs)[searchSelection[j].terrNo].getDefenders();
			if (troopType)
				troop = (*allTerrs)[searchSelection[j].terrNo].getAttackers(false);

			if (troop != 0) {
				reinforcingTerrs.push_back(searchSelection[j].terrNo);
				if (difference - troop <= 0) {
					difference = 0;
					break;
				} else {
					difference -= troop;
				}
			}
		}
	}
}

void AI::findAttackTerrs(int &difference, vector<int> &attackingTerrs, TerrInfo &toAttack, vector<TerrInfo> &touchingTerrs, vector<TerrInfo> &searchSelection) {
	for (unsigned int j = 0; j < searchSelection.size(); j++) {

		if (find_if(toAttack.enemyBorders.begin(), toAttack.enemyBorders.end(), findTerr(searchSelection[j].terrNo)) !=
			toAttack.enemyBorders.end()) {

			touchingTerrs.push_back(searchSelection[j]);

			int atk;
			if ((atk = (*allTerrs)[searchSelection[j].terrNo].getAttackers(false)) != 0) {

				attackingTerrs.push_back(searchSelection[j].terrNo);
				if (difference - atk <= 0) {
					difference = 0;
					break;
				} else {
					difference -= atk;
				}
			}
		}
	}
}

void AI::handleDefence() {
	
	sort(ourVuln.begin(), ourVuln.end());		//Sort by population
	for (int i = (int) (ourVuln.size()) - 1; i >= 0; i--) {
		int def;
		int difference = (int) ceil((ourVuln[i].maxAtk - ourVuln[i].defence) / 15) + 1;	//Find how many defenders we need to be safe
		if (difference < 0) continue;
		vector<int> reinforcingTerrs;

		findReinforceTerrs(difference, reinforcingTerrs, ourVuln[i], ourSafe, false);
		int r = rand() % 2;
		if (r != 1) {
			if (difference > 0) {	//Buy defenders
				def = ourVuln[i].population - 10;
				if (def > (*ai).coins / 50) {
					def = (*ai).coins / 50;
				}

				if (difference - def < 0) {
					def = difference;
					difference = 0;
				} else {
					difference -= def;
				}
				reinforcingTerrs.push_back(-def);
			}
		}

		if (difference > 0) {
			findReinforceTerrs(difference, reinforcingTerrs, ourVuln[i], ourTerrs, false);
		}

		if (difference > 0) {
			for (unsigned int j = i - 1; j > -1; j--) {	//Go through the other vulnerable territories, last resort

				if (find_if(ourVuln[i].friendlyBorders.begin(), ourVuln[i].friendlyBorders.end(), findTerr(ourVuln[j].terrNo)) !=
					ourVuln[i].friendlyBorders.end()) {	

					if ((def = (*allTerrs)[ourVuln[j].terrNo].getDefenders()) != 0) {	

						reinforcingTerrs.push_back(ourVuln[j].terrNo);
						if (difference - def < 0) {
							difference = 0;
							break;
						} else {
							difference -= def;
						}
					}
				}
			}
		}

		if (difference == 0) {	//If we can defend this territory, do so
			difference = (int)ceil((ourVuln[i].maxAtk - ourVuln[i].defence) / 15) + 1;
			for (unsigned j = 0; j < reinforcingTerrs.size(); j++) {	//Goes through all our reinforcements
				if (reinforcingTerrs[j] > 0) {
					int defSend = (*allTerrs)[reinforcingTerrs[j]].getDefenders();
					if (defSend >= difference) {
						(*allTerrs)[reinforcingTerrs[j]].sendTroops((*allTerrs)[ourVuln[i].terrNo], 0, difference);
					} else {
						(*allTerrs)[reinforcingTerrs[j]].sendTroops((*allTerrs)[ourVuln[i].terrNo], 0, defSend);
						difference -= defSend;
					}
				} else {	//Negative values mean they are to be purchased
					int toBuy = reinforcingTerrs[j] * -1;
					(*ai).coins -= toBuy * 100;
					for (int k = 0; k < toBuy; k++) 
						(*allTerrs)[ourVuln[i].terrNo].addDefender();
					
				}
			}
		} else {	//If this territory cannot gain high enough defence

			int noAtk = (*allTerrs)[ourVuln[i].terrNo].getAttackers(false);
			int noDef = (*allTerrs)[ourVuln[i].terrNo].getDefenders();

			if (i > 0) {	//if there are vulnerable territories that we haven't tried to save, send them the troops we have (to save them being destroyed)

				(*allTerrs)[ourVuln[i].terrNo].sendTroops((*allTerrs)[ourVuln[i - 1].terrNo], noAtk, noDef);
				ourVuln[i - 1] = (*allTerrs)[ourVuln[i - 1].terrNo].getAIData((*allTerrs));

			} else {	//If there are no more vulnerable territories, look for another territory and send on the attackers

				for (unsigned int j = 0; j < ourTerrs.size(); j++) {	//Go through our not unsafe territories

					if (find_if(ourVuln[i].friendlyBorders.begin(), ourVuln[i].friendlyBorders.end(), findTerr(ourTerrs[j].terrNo)) !=
						ourVuln[i].friendlyBorders.end()) {
						(*allTerrs)[ourVuln[i].terrNo].sendTroops((*allTerrs)[ourTerrs[j].terrNo], noAtk, 0);
					}
				}

				for (unsigned int j = 0; j < ourSafe.size(); j++) {	//Go through our safe territories

					if (find_if(ourVuln[i].friendlyBorders.begin(), ourVuln[i].friendlyBorders.end(), findTerr(ourSafe[j].terrNo)) !=
						ourVuln[i].friendlyBorders.end()) {	//if our vulnerable terr shares a border with a safe
						(*allTerrs)[ourVuln[i].terrNo].sendTroops((*allTerrs)[ourSafe[j].terrNo], noAtk, 0);			
					}
				}
			}
		}
	}
	fillArrays();
}

void AI::attack(vector<TerrInfo> &toAttack) {
	sort(toAttack.begin(), toAttack.end());		//Sort by population

	for (int i = (int)(toAttack.size()) - 1; i >= 0; i--) {
		int defence = (int)(ceil(toAttack[i].defence / 15) + 1);
		bool attackNow = true, sucess = false;
		vector<int> attackingTerrs, reinforcingTerrs;
		vector<TerrInfo> touchingTerrs;

		findAttackTerrs(defence, attackingTerrs, toAttack[i], touchingTerrs, borders);

		if (defence > 0) {
			attackNow = false;
			for (unsigned int j = 0; j < touchingTerrs.size(); j++) {

				int defenceBefore = defence;
				findReinforceTerrs(defence, reinforcingTerrs, touchingTerrs[j], inners, true);
				int defenceDif = defenceBefore - defence;

				for (unsigned int k = 0; k < reinforcingTerrs.size(); k++) {
					(*allTerrs)[reinforcingTerrs[k]].sendTroops((*allTerrs)[touchingTerrs[j].terrNo], defenceDif, 0);
				}
				reinforcingTerrs.clear();
			}
		}

		if (defence > 0) {	//Buy attackers
			for (unsigned int j = 0; j < touchingTerrs.size(); j++) {
				Territory &tt = (*allTerrs)[touchingTerrs[j].terrNo];

				int buy = touchingTerrs[j].population - 10;
				if (buy > (*ai).coins / 50) {
					buy = (*ai).coins / 50;
				}

				if (defence - buy < 0) {
					buy = defence;
					defence = 0;
				} else {
					defence -= buy;
				}

				for (int k = 0; k < buy; k++) {
					(*ai).coins -= 50;
					tt.addAttacker();
				}
				if (defence == 0) break;
			}
		}

		defence = (int)(ceil(toAttack[i].defence / 15) + 1);
		int r = rand() % 3;
		if (r != 2) {
			for (unsigned j = 0; j < attackingTerrs.size(); j++) {

				Territory &terr = (*allTerrs)[attackingTerrs[j]];
				int atk = terr.getAttackers(false);

				if (defence <= atk) {
					atk = defence;
					defence = 0;
				} else {
					defence -= atk;
				}

				if (attackNow)
					terr.sendTroops((*allTerrs)[toAttack[i].terrNo], atk, 0);
				if ((*allTerrs)[toAttack[i].terrNo].getOwner() == 1) sucess = true;
			}
		}

		if (sucess) toUpdateTerrNo.push_back(toAttack[i].terrNo);
	}
}

void AI::handleAttack() {
	attack(theirVuln);
	attack(enemyBorders);
	attack(theirSafe);
}

void AI::addBuildings() {

	int totalBuildings = (*ai).coins / 500;
	int maxBanks = (int)(totalBuildings / 2), maxFarms = (int)ceil(totalBuildings / 2);

	if (farms < maxFarms) {
		if (banks < maxBanks + (maxFarms - farms)) {
			maxBanks = banks;
		} else {
			maxBanks += (maxFarms - farms);
		}
		maxFarms = farms;
	}

	if (banks < maxBanks) {
		if (farms < maxFarms + (maxBanks - banks)) {
			maxFarms = farms;
		} else {
			maxFarms += (maxBanks - banks);
		}
		maxBanks = banks;
	}

	sort(inners.begin(), inners.end());
	for (int i = (int)(inners.size()) - 1; i >= 0; i--) {
		if (maxBanks > 0) {
			if (!inners[i].bank) {
 				(*bankArray).push_back(inners[i].terrNo);
				(*ai).coins -= 500;
				maxBanks--;
			} 
		} else {
			break;
		}
	}
	for (unsigned int i = 0; i < inners.size(); i++) {
		if (maxFarms > 0) {
			if (!inners[i].farm) {
				(*farmArray).push_back(inners[i].terrNo);
				(*ai).coins -= 500;
				maxFarms--;
			}
		} else {
			break;
		}
	}
}