#include <iostream>
#include <fstream>
#include <math.h>
#include <chrono>
#include "Game.h"
#include "Shaders.h"
#include "Textures.h"
#include "log.h"

using namespace std;

void Game::initGame() {

	//gameMusic.playAudio("sound.wav");
	gameMusic.setVolume(100);
	swordClang.setVolume(100);

	mm.initMenu();
	//newGame();
}

#pragma region save
vector<int> Game::getSave() {

	vector<int> saveCode;

	/*
		Opens your file for reading. Look up fstream if confused. Basically allows for simpler file handling.
	*/
	ifstream file("Saves\\save1.hexatories");
	if (!file.is_open()) {
		saveCode.push_back(-1);
		return saveCode;
	}

	string saveString;

	string line;
	getline(file, line);
	saveString.append(line);

	file.close();

	vector<string> subStrings;	//Used to store all the strings between comma delimiters
	int delPos;

	/*
		Loops through the whole string deleting commas and splitting what is between the commas into separate strings
	*/
	while (!saveString.empty()) {

		delPos = saveString.find(",");	//Find where the comma is
		subStrings.push_back(saveString.substr(0, delPos));	//Copy up to (but excluding the comma)
		saveString.erase(0, delPos + 1);	//Erase everything up to (including the comma)

	}

	/*
		Converts the sub strings to their integer values
	*/
	for (unsigned int i = 0; i < subStrings.size(); i++) 
		saveCode.push_back(atoi(subStrings[i].c_str()));
	
	return saveCode;
}

void Game::newGame() {

	territories.clear();

	map.initMap(territories, true);	//true so that map will init the territories
	ui.initUI();

	for (int i = 0; i < 2; i++) {
		players[i].coins = 250;
		players[i].score = 0;
		players[i].population = 0;
	}

	turnNo = 1;

	updatePlayerInfo();
}

bool Game::loadGame() {

	vector<int> save = getSave();	// save data
	if (save[0] == -1) {
		newGame();
		return false;
	}

	territories.clear();

	map.initMap(territories, false);	//false so map doesn't init territories
	ui.initUI();
	
	vector<int> buildings[2];	//keeps the territory numbers that have [0] farms and [1] banks built

	turnNo = save[0];
	players[0].coins = save[1];
	players[1].coins = save[2];

	Territory temp;

	for (unsigned int i = 3; i < save.size(); i) {	//i is incremented as we go as each territory is of a different size
		
		territories.push_back(temp);	//blank territory
		int terrNo = territories.size() - 1;	//calculate terrNo
		int owner = save[i++];	//get territory owner

		vector<tile_t> tiles[2];	//[0] border tiles [1] inner tiles

		for (int k = 0; k < 2; k++) {	//loops through both types of tile
			int numTiles = save[i++];	//number of tiles (of a type)

			for (int j = 0; j < numTiles; j++) {	//loop through that amount of numbers * 2, as each tile has an x and a y

				tile_t newTile = { save[i++], save[i++], terrNo };	//x, y, terrNo
				tiles[k].push_back(newTile);
				map.addToTiles(newTile);	//adds the tile to the maps array of them so that we can get it later (for mouse input)

			}
		}

		int attackers[2];
		int defenders[2];

		for (int j = 0; j < 2; j++) {
			attackers[j] = save[i++];
			defenders[j] = save[i++];
		}

		int pop = save[i++];
		if (owner != 0) players[owner - 1].population += pop;	//adds population to total pop of relevant player

		int farm = save[i++];	//will be -1 if territory did not have
		int bank = save[i++];

		if (farm != -1) buildings[0].push_back(terrNo);
		if (bank != -1) buildings[1].push_back(terrNo);

		territories[terrNo].initTerritory(owner, tiles[0], tiles[1], attackers, defenders, pop);	//init territory with read info
	}

	for (unsigned int i = 0; i < territories.size(); i++)	//updates all territories borders on map
		map.updateBorder(territories[i], true);
	
	for (unsigned int i = 0; i < buildings[0].size(); i++)	// updates all territories with farms
		map.updateBuilding(&territories[buildings[0][i]], true);
	
	for (unsigned int i = 0; i < buildings[1].size(); i++)	//updates all territories with banks
		map.updateBuilding(&territories[buildings[1][i]], false);

	updatePlayerInfo();
	ui.changeText(gameUI::Text::ROUND, turnNo);

	return true;
}

void Game::saveGame() {

	string saveCode;

	saveCode.append(to_string(turnNo));	//adds turn number
	saveCode.append(",");	//commas separate the values

	/*
		Adds players coins values
	*/
	for (int i = 0; i < 2; i++) {
		saveCode.append(to_string(players[i].coins));
		saveCode.append(",");
	}
	/*
		Loops through each territory filling the save data
	*/
	for (unsigned int i = 0; i < territories.size(); i++)
		territories[i].fillSaveData(saveCode);

	ofstream file("Saves\\save1.hexatories");

	if (!file.is_open()) {
		return;
	}

	file << saveCode;
	file.close();
}
#pragma endregion

#pragma region inputHandle
bool Game::handleMouseInput(double x, double y, bool click, bool reset) {

	static Territory *firstTerr, *secondTerr, *currTerr;	//firstTerr & secondTerr for send troops, currTerr for the selected one
	static int numAtkSend, numDefSend;	//The number of troops selected with the arrows for send trops
	static bool sendTroopsPressed = false;	//So we know what to do when the button is pressed next
	static bool settingsOpen = false;

	int ix = (int)x;
	int iy = (int)y;

#pragma region reset
	if (reset) {

		if (secondTerr != NULL) selectTerr(NULL, secondTerr);
		if (currTerr != NULL) selectTerr(NULL, currTerr);

		currTerr = NULL;
		firstTerr = NULL;
		secondTerr = NULL;

		if (sendTroopsPressed) {
			ui.changeButton(-1);
			sendTroopsPressed = false;
		}
		if (settingsOpen) {
			settingsOpen = false;
			ui.drawSettings(0);
		}
		return false;
	}
#pragma endregion

#pragma region mainMenu
	if (stage == 1 && click) if (handleMainMenu(ix, iy)) return true;
	if (stage != 3) return false;
#pragma endregion

#pragma region gameInput
	gameUI::Section sect = ui.pointInBox(ix, iy);

#pragma region Settings
	if (settingsOpen) {

		if (!click && sect != gameUI::Section::NULL_SEC) return false;

		switch (sect) {

		case gameUI::Section::SETTINGS:
		case gameUI::Section::NULL_SEC: {
			settingsOpen = false;
			ui.drawSettings(0);
			break;
		}

		case gameUI::Section::MUTE_BACK: {
			ui.drawSettings(1);
			if (gameMusic.getVolume() == 100) {
				gameMusic.setVolume(0);
			} else {
				gameMusic.setVolume(100);
			}
			break;
		}

		case gameUI::Section::MUTE_EFFECTS: {
			ui.drawSettings(2);
			if (swordClang.getVolume() == 100) {
				swordClang.setVolume(0);
			} else {
				swordClang.setVolume(100);
			}
			break;
		}

		case gameUI::Section::SAVE: {
			saveGame();
			break;
		}

		case gameUI::Section::EXIT_WINDOWS: {
			return true;
		}

		case gameUI::Section::EXIT_MAIN: {
			handleMouseInput(0, 0, false, true);
			stage = 0;
			mm.initMenu();
			break;
		}

		}
	} else {
#pragma endregion

		/*
		Gets the section our mouse coords are within, if it's on the map & is a mouseover highlights the tile
		*/
		if (sect == gameUI::Section::MAP && !click) {
			Territory *hTerr;
			hTerr = getTerritory(ix, iy);
			highlightTerritory(hTerr);
			return false;
		}

		highlightTerritory(NULL);	//Will only reach here if not on map or is a click, therefore get rid of the previously highlighter terr

		if (!click) return false;	//Everything beyond this point requires a click

		switch (sect) {	//Based on button pressed

		case gameUI::Section::MAP: {

			if (!sendTroopsPressed) {	//If we haven't pressed send troops (we are just selecting a territory)

				Territory *nextTerr = getTerritory(ix, iy);	//Get the clicked on territory
				if (nextTerr == currTerr) nextTerr = NULL;	//If this and previous are the same we want to deselect it
				selectTerr(nextTerr, currTerr);	//Passing NULL as the first parameter just deselects the second, if not swaps which is selected
				currTerr = nextTerr;

			} else {	//Send troops pressed, we have one territory (to send from) selected, firstTerr

				if (secondTerr != NULL) selectTerr(NULL, secondTerr);	//If we have already selected a secondTerr, we are changing it, so NULL

				secondTerr = getTerritory(ix, iy);	//Get the territory we clicked on
				if (secondTerr == firstTerr) {	//If it is the one we wanted to send from

					selectTerr(NULL, firstTerr);	//Deselect it
					firstTerr = NULL;	//Reset all territories
					secondTerr = NULL;
					currTerr = NULL;

					/*
						Resets the send troops button to default and resets state
						*/
					ui.changeButton(-1);
					sendTroopsPressed = false;

					break;
				}

				selectTerr(secondTerr, NULL);	//If valid select the territory

				/*
					If they have different owners, change button to attack, if not change to send troops (indented)
					*/
				if (secondTerr->getOwner() != firstTerr->getOwner()) {
					ui.changeButton(1);
				} else if (secondTerr->getOwner() == firstTerr->getOwner()) {
					ui.changeButton(0);
				}
			}
			break;
		}

		case gameUI::Section::SEND_TROOPS: {

			if (currTerr == NULL) break;	//If no terr selected

			if (!sendTroopsPressed) {	//If the button hasn't been pressed yet
				if (currTerr->getOwner() == 2) {
					firstTerr = currTerr;	//Remember first terr selected
					sendTroopsPressed = true;
					ui.changeButton(0);	//Indent button
				}

			} else if (secondTerr != NULL) {

				ui.changeButton(-1);	//Resets the button

				firstTerr->sendTroops(*secondTerr, 5, 0);	//Send troops

				selectTerr(NULL, firstTerr);	//deselect territories
				selectTerr(NULL, secondTerr);

				firstTerr = NULL;	//reset all territories
				secondTerr = NULL;
				currTerr = NULL;

				numAtkSend = 0;	//reset troops to send
				numDefSend = 0;

				updatePlayerInfo();

				ui.changeText(gameUI::Text::SEND_ATK, 0);	//update troops to send text
				ui.changeText(gameUI::Text::SEND_DEF, 0);
				sendTroopsPressed = false;
			}
			break;
		}

		case gameUI::Section::ATK_UP: {
			ui.changeText(gameUI::Text::SEND_ATK, ++numAtkSend);	//Add one to the attack to send text & var
			break;
		}

		case gameUI::Section::ATK_DOWN: {
			if (numAtkSend > 0)
				ui.changeText(gameUI::Text::SEND_ATK, --numAtkSend);	//Take one from the attack to send text & var
			break;
		}

		case gameUI::Section::DEF_UP: {
			ui.changeText(gameUI::Text::SEND_DEF, ++numDefSend);	//Add one to the defence to send text & var
			break;
		}

		case gameUI::Section::DEF_DOWN: {
			if (numDefSend > 0)
				ui.changeText(gameUI::Text::SEND_DEF, --numDefSend);	//Take one from the attack to send text & var
			break;
		}

		case gameUI::Section::BUY_FARM: {
			if (currTerr->getOwner() == 1) {
				if (players[1].coins >= 1500) {
					map.updateBuilding(currTerr, true);	//Adds a farm to the territory & map
					players[1].coins = players[1].coins - 1500;
				}
			}
			break;
		}

		case gameUI::Section::BUY_BANK: {
			if (currTerr->getOwner() == 1) {
				if (players[1].coins >= 1500) {
					map.updateBuilding(currTerr, false);	//Adds a bank to the territory & map
					players[1].coins = players[1].coins - 1500;
				}
				break;
			}

		case gameUI::Section::SETTINGS: {
			settingsOpen = true;
			ui.drawSettings(0);
			break;
		}

		}
		}
	}
	return false;
#pragma endregion
}

bool Game::handleMainMenu(int x, int y) {
	MainMenu::Section sect = mm.pointInBox(x, y);

	switch (sect) {

	case (MainMenu::Section::NEW_GAME) : {
		stage = 2;
		mm.changeScreen(2);
		return false;
	}

	case (MainMenu::Section::LOAD_GAME) : {
		stage = 4;
		mm.changeScreen(2);
		return false;
	}

	case (MainMenu::Section::TUTORIAL) : {
		break;
	}

	case (MainMenu::Section::EXIT) : {
		return true;
	}

	}
	return false;
}

void Game::handleKeyInput(int key) {

	if (key == GLFW_KEY_SPACE) {

		if (stage == 0) {
			mm.changeScreen(1);
			stage = 1;
		}
		if (stage == 3)
			nextTurn();	//Move turn on when space pressed
		if (stage == 2) {
			newGame();
			stage = 3;
		}
		if (stage == 4) {
			loadGame();
			stage = 3;
		}
	}

}
#pragma endregion

#pragma region territoryHandling
Territory *Game::getTerritory(int x, int y) {
	tile_t currTile = map.pointToTile(x, y);	//Gets the tile @ mouse coords

	if (currTile.terrNo == -1) return NULL;	// if not a valid tile

	return &territories[currTile.terrNo];
}

void Game::selectTerr(Territory *terr, Territory *prevTerr) {

	int vals[5];

	if (prevTerr != NULL) {	
		changeTerritoryColour(*prevTerr, -1);	//Default colour (that of the owner)
		prevTerr->invSelect();	//Not selected anymore
	}

	if (terr == NULL) {	//If we aren't selecting a new territory
		for (int i = 0; i < 5; i++) vals[i] = 0;	//all values are 0
	} else {
		changeTerritoryColour(*terr, 3);	//Change to gold
		terr->invSelect();	//Selected now
		terr->getInfo(vals);	//get values
	}

	ui.changeText(gameUI::Text::POP, vals[0]);	//update text on sidebar
	ui.changeText(gameUI::Text::ATK, vals[1]);
	ui.changeText(gameUI::Text::DEF, vals[2]);
	ui.changeText(gameUI::Text::TOTAL_ATK, vals[3]);
	ui.changeText(gameUI::Text::TOTAL_DEF, vals[4]);
}

void Game::highlightTerritory(Territory *currTerr) {
	static Territory *prevTerr;	//Remembers the currently highlighted territory

	if (currTerr == prevTerr) return;

	if (prevTerr != NULL)	//if we have a prev highlighted terr
		if (!prevTerr->isSelected())	//if it is not selected
			changeTerritoryColour(*prevTerr, -1);	//default colour

	if (currTerr != NULL) {	//If we have a valid terr to highlight

		prevTerr = currTerr;
		changeTerritoryColour(*prevTerr, 3); // change to gold
		return;
	}

	prevTerr = NULL;	//Only reached if no highlighted territory
}

void Game::changeTerritoryColour(Territory &ter, int col) {
	ter.setColour(col);	//Sets the colour (-1 being default)
	map.updateBorder(ter, false);	//Gets the map to update to new colour
}
#pragma endregion

void Game::draw() {

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (stage == 3) {
		map.drawMap();
		ui.drawUI();
	} else {
		mm.drawMenu();
	}
}

void Game::nextTurn() {
	ui.changeText(gameUI::Text::ROUND, ++turnNo);	//increment turn
 	for (unsigned int i = 0; i < territories.size(); i++) {

		int owner, coins = -1;
		owner = territories[i].getOwner();
		owner--;

		if (owner != -1) 
			coins = players[owner].coins;
		

		territories[i].incrementTurn(coins);

		if (owner > 0) {
			players[owner].coins = coins;
			if (players[owner].coins > 99999) players[owner].coins = 99999;
		}
	}

	updatePlayerInfo();

	handleMouseInput(0, 0, false, true);
}

void Game::updateScore(int player) {

}

void Game::updatePlayerInfo() {

	players[1].population = 0;

	for (unsigned int i = 0; i < territories.size(); i++)
		if (territories[i].getOwner() == 2)
			players[1].population += territories[i].getPopulation();

	ui.changeText(gameUI::Text::COINS, players[1].coins);
	ui.changeText(gameUI::Text::TOTAL_POP, players[1].population);
	ui.changeText(gameUI::Text::SCORE, players[1].score);
}