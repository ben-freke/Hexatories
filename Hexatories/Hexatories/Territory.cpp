#include "Territory.h"
using namespace std;



void Territory::initTerritory(vector<int> innerTiles, int tileCount) {

	size = tileCount;
	tiles = innerTiles;
	

	

	//setupBorderTiles();
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