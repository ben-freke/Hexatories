#include <algorithm>
#include "Territory.h"
#include "log.h"

using namespace std;

void Territory::initTerritory(vector<tile_t> innerTiles, int tileCount) {

	size = tileCount;

	setupTiles(innerTiles);
}

void Territory::setupTiles(vector<tile_t> innerTiles) {

	int x, y, xoff, adjacentTiles;
	int xChange[6] = { 0, 0, 1, 1, -1, -1 };
	int yChange[6] = { -1, 1, -1, 0, -1, 0 };

	for (int i = 0; i < size; i++) {

		x = innerTiles[i].x;
		y = innerTiles[i].y;
		xoff = x % 2;
		adjacentTiles = 0;

		for (int j = 0; j < 6; j++) {
			if (find_if(innerTiles.begin(), innerTiles.end(), findTile(x + xChange[j], y + yChange[j] + xoff * abs(xChange[j]))) != innerTiles.end()) {
				adjacentTiles++;
			}
		}

		if (adjacentTiles == 6) {
			tiles.push_back(innerTiles[i]); 
		} else {
			border.push_back(innerTiles[i]);
		}
	}
}

int Territory::getAttackScore(){
	return troopsAttack;
}

void Territory::setAttackScore(int value){
	troopsAttack = value;
}

int Territory::getDefenseScore(){
	return troopsDefense;
}

void Territory::setDefenseScore(int value){
	troopsDefense =  value;
}