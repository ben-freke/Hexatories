#include <iostream>
#include <math.h>
#include <chrono>
#include "Game.h"
#include "Shaders.h"
#include "Textures.h"
#include "log.h"

using namespace std;

bool Game::initGame() {

	if (!map.initMap(territories))
		return false;
	ui.initUI();
	return true;
}

void Game::highlightTerritory(double x, double y) {
	static tile_t prevTile = { -1, -1, -1 };
	tile_t currTile = map.pointToTile(x, y);

	if (currTile.x != -1) {
		if (!(currTile == prevTile)) {

			if (prevTile.x != -1) 
				changeTerritoryColour(territories[prevTile.terrNo], -1);

			prevTile = currTile;
			changeTerritoryColour(territories[prevTile.terrNo], 3);
		}
	} else if (prevTile.x != -1) {
		changeTerritoryColour(territories[prevTile.terrNo], -1);
		prevTile.x = -1;
	}
}

Territory *Game::getTerritory(double x, double y){
	tile_t currTile = map.pointToTile(x, y);
	
	if (currTile.terrNo == -1) return NULL;
	
	return &territories[currTile.terrNo];
}

void Game::changeTerritoryColour(Territory &ter, int col) {
	ter.setColour(col);
	map.updateVBO(ter);
}

void Game::draw() {
	map.drawMap();
	ui.drawUI();
}

void Game::resetTerrs() {
	for (unsigned int i = 0; i < territories.size(); i++) territories[i].reset();
}

/*
	Sends the troops from one tile to another. METHOD IS NOT YET FOOLPROOF.
	Takes in a sending and receiving territory, the type of troop (e.g. attackers or defenders) and the 
	number of troops to send.
*/

void Game::sendTroops(Territory &receivingTerr, Territory &sendingTerr, int troopType, int noTroops){
	/*
		Troop type can be either 1 or 0, where 1 is attackers and 0 is defenders.
	*/
	if (troopType == 0){

		//Send Defender


	}
	else if (troopType == 1) {
		if (sendingTerr.getAttackers() >= noTroops) {
			if (receivingTerr.getOwner() != sendingTerr.getOwner()) {

				int attack = noTroops * 15;
				int defense = receivingTerr.getDefense();

				int randomBoundaries = (int)(attack * 0.10);
				int randomValue = rand() % randomBoundaries - (int)(randomBoundaries / 2);
				int difference = defense - (attack + randomValue);

				int remainingTroops = (int)ceil(difference / 15);

				/*
					Calculate remaining troops. There will only be one type of troop remaining, either attackers
					or defenders. The type of troop remaining is dependant on the outcome of the battle.
					*/
				/*
					The attack has been lost. Only defenders remain.
					*/
				if (difference > 0) {
					/*
						Destroy all attackers sent from the opposing territory
						*/
					sendingTerr.destroyAttackers(noTroops);

					/*
						Destroys any lost defenders in the territory
					*/
					receivingTerr.destroyDefenders(noTroops);
				}
				/*
					Attack has won. All defenders should be destroyed and remaining attackers recorded.
					*/
				else {
					remainingTroops = remainingTroops * -1;

					/*
					Destroys any defenders remaining in the territory
					*/
					receivingTerr.destroyDefenders(-1);
					receivingTerr.destroyAttackers(-1);

					/*
					Destroy all attackers sent from the opposing territory
					*/
					sendingTerr.destroyAttackers(noTroops - remainingTroops);

					sendingTerr.sendTroops(receivingTerr, remainingTroops, 0);

					receivingTerr.setOwner(sendingTerr.getOwner());
					changeTerritoryColour(receivingTerr, -1);

					cout << "Success in attacking\n";
				}
			} else {
				cout << "You have the same owner\n";
			}
		}
		else{
				cout << "You do not have enough troops to send\n";
		}
	}
}
