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

void Game::handleMouseInput(double x, double y, bool click) {
	static Territory *firstTerr, *secondTerr, *currTerr;	//firstTerr & secondTerr for send troops, currTerr for the selected one
	static int numAtkSend, numDefSend;
	static bool sendTroopsPressed = false, attack = false;

	gameUI::Section sect;

	int ix = (int)x;
	int iy = (int)y;

	if (((sect = ui.pointInBox(ix, iy)) == gameUI::Section::MAP) &! click) {
		Territory *hTerr;
		hTerr = getTerritory(ix, iy);
		highlightTerritory(hTerr);
		return;
	}

	highlightTerritory(NULL);
	if (!click) return;

	switch (sect) {

	case gameUI::Section::MAP:{

		if (firstTerr == NULL) {

			Territory *nextTerr = getTerritory(ix, iy);
			if (nextTerr == currTerr) currTerr = NULL;
			selectTerr(nextTerr, currTerr);
			currTerr = nextTerr;

		} else {

			if (secondTerr != NULL) selectTerr(NULL, secondTerr);

			secondTerr = getTerritory(ix, iy);
			if (secondTerr == firstTerr) {

				selectTerr(NULL, firstTerr);
				firstTerr = NULL;
				secondTerr = NULL;
				sendTroopsPressed = false;

				if (attack) {
					ui.attackButton(false);
					attack = false;
				}

				break;

			}

			selectTerr(secondTerr, NULL);

			if (secondTerr->getOwner() != firstTerr->getOwner() && !attack) {
				ui.attackButton(true);
				attack = true;
			} else if (secondTerr->getOwner() == firstTerr->getOwner() && attack) {
				ui.attackButton(false);
				attack = false;
			}
		}
		break;
	}

	case gameUI::Section::SEND_TROOPS: {
		if (!sendTroopsPressed && currTerr != NULL) {
			firstTerr = currTerr;
			sendTroopsPressed = true;
		} else {

			if (attack) {
				ui.attackButton(false);
				attack = false;
			}

			sendTroops(*secondTerr, *firstTerr, 1, 5);
			selectTerr(NULL, firstTerr);
			selectTerr(NULL, secondTerr);
			firstTerr = NULL;
			secondTerr = NULL;

			numAtkSend = 0;
			numDefSend = 0;
			ui.changeText(gameUI::Text::SEND_ATK, 0);
			ui.changeText(gameUI::Text::SEND_DEF, 0);
			sendTroopsPressed = false;
		}
		break;
	}

	case gameUI::Section::ATK_UP: {
		ui.changeText(gameUI::Text::SEND_ATK, ++numAtkSend);
		break;
	}

	case gameUI::Section::ATK_DOWN: {
		if (numAtkSend > 0)
			ui.changeText(gameUI::Text::SEND_ATK, --numAtkSend);
		break;
	}

	case gameUI::Section::DEF_UP: {
		ui.changeText(gameUI::Text::SEND_DEF, ++numDefSend);
		break;
	}

	case gameUI::Section::DEF_DOWN: {
		if (numDefSend > 0)
			ui.changeText(gameUI::Text::SEND_DEF, --numDefSend);
		break;
	}

	case gameUI::Section::BUY_FARM: {
		map.updateBuilding(currTerr, true);
		break;
	}

	case gameUI::Section::BUY_BANK: {
		map.updateBuilding(currTerr, false);
		break;
	}

	}
}

void Game::selectTerr(Territory *terr, Territory *prevTerr) {

	int vals[3];

	if (prevTerr != NULL) {
		changeTerritoryColour(*prevTerr, -1);
		prevTerr->invSelect();
	}

	if (terr == NULL) {
		for (int i = 0; i < 3; i++) vals[i] = 0;
	} else {
		changeTerritoryColour(*terr, 3);
		terr->invSelect();
		terr->getInfo(vals);
	}

	ui.changeText(gameUI::Text::POP, vals[0]);
	ui.changeText(gameUI::Text::ATK, vals[1]);
	ui.changeText(gameUI::Text::DEF, vals[2]);
}

void Game::highlightTerritory(Territory *currTerr) {
	static Territory *prevTerr;

	if (currTerr != NULL) {
		if (!(currTerr == prevTerr)) {

			if (prevTerr != NULL)
				if (!prevTerr->isSelected()) 
					changeTerritoryColour(*prevTerr, -1);

			prevTerr = currTerr;
			changeTerritoryColour(*prevTerr, 3);
		}
	} else if (prevTerr != NULL) {
		if (!prevTerr->isSelected()) {
			changeTerritoryColour(*prevTerr, -1);
			prevTerr = NULL;
		}
	}
}

Territory *Game::getTerritory(int x, int y){
	tile_t currTile = map.pointToTile(x, y);
	
	if (currTile.terrNo == -1) return NULL;
	
	return &territories[currTile.terrNo];
}

void Game::changeTerritoryColour(Territory &ter, int col) {
	ter.setColour(col);
	map.updateBorder(ter);
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
