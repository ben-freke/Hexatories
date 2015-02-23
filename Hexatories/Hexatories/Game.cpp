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
	newGame();
	players[0].coins = 1500;
	players[1].coins = 1500;
}

#pragma region save
vector<int> Game::getSave() {

	string savePath;
	cout << "Input full save path & filename\nE.G. C:\\Games\\Hexatories\\save.save\n";
	cin >> savePath;

	/*
		Opens your file for reading. Look up fstream if confused. Basically allows for simpler file handling.
	*/
	ifstream file(savePath);
	while (!file.is_open()) {
		savePath.clear();
		cout << "Input full save path & filename\nE.G. C:\\Games\\Hexatories\\save.test\n";
		cin >> savePath;
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

	vector<int> saveCode;

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

}

void Game::loadGame() {

	territories.clear();

	map.initMap(territories, false);	//false so map doesn't init territories
	ui.initUI();

	vector<int> save = getSave();	// save data
	
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

	ui.changeText(gameUI::Text::COINS, players[1].coins);
	ui.changeText(gameUI::Text::SCORE, players[1].score);
	ui.changeText(gameUI::Text::TOTAL_POP, players[1].population);
	ui.changeText(gameUI::Text::ROUND, turnNo);
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

	string savePath;
	cout << "Input full save path & filename\nE.G. C:\\Games\\Hexatories\\save.save\n";
	cin >> savePath;

	ofstream file(savePath);

	while (!file.is_open()) {
		savePath.clear();
		cout << "Input full save path & filename\nE.G. C:\\Games\\Hexatories\\save.save\n";
		cin >> savePath;
		ofstream file(savePath);
	}

	file << saveCode;
	file.close();
}
#pragma endregion

#pragma region inputHandle
void Game::handleMouseInput(double x, double y, bool click, bool reset) {

	static Territory *firstTerr, *secondTerr, *currTerr;	//firstTerr & secondTerr for send troops, currTerr for the selected one
	static int numAtkSend, numDefSend;	//The number of troops selected with the arrows for send trops
	static bool sendTroopsPressed = false;	//So we know what to do when the button is pressed next

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
	}

	gameUI::Section sect;

	int ix = (int)x;
	int iy = (int)y;

	/*
		Gets the section our mouse coords are within, if it's on the map & is a mouseover highlights the tile 
	*/
	if (((sect = ui.pointInBox(ix, iy)) == gameUI::Section::MAP) &! click) {
		Territory *hTerr;
		hTerr = getTerritory(ix, iy);
		highlightTerritory(hTerr);
		return;
	}

	highlightTerritory(NULL);	//Will only reach here if not on map or is a click, therefore get rid of the previously highlighter terr

	if (!click) return;	//Everything beyond this point requires a click

	switch (sect) {	//Based on button pressed

	case gameUI::Section::MAP:{

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
			
			firstTerr = currTerr;	//Remember first terr selected
			sendTroopsPressed = true;
			ui.changeButton(0);	//Indent button

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

			ui.changeText(gameUI::Text::SEND_ATK, 0);	//update troops to send text
			ui.changeText(gameUI::Text::SEND_DEF, 0);
			sendTroopsPressed = false;
		}
		break;
	}

	case gameUI::Section::ATK_UP: {
		ui.changeText(gameUI::Text::SEND_ATK, ++numAtkSend);	//Add one to the attack to send text & var
		saveGame();
		break;
	}

	case gameUI::Section::ATK_DOWN: {
		if (numAtkSend > 0)
			ui.changeText(gameUI::Text::SEND_ATK, --numAtkSend);	//Take one from the attack to send text & var
		loadGame();
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
		if (players[1].coins > 1500){
			map.updateBuilding(currTerr, true);	//Adds a farm to the territory & map
			players[1].coins = players[1].coins - 1500;
		}
		break;
	}

	case gameUI::Section::BUY_BANK: {
		if (players[1].coins > 1500){
			map.updateBuilding(currTerr, false);	//Adds a bank to the territory & map
			players[1].coins = players[1].coins - 1500;
		}
		break;
	}

	}
}

void Game::handleKeyInput(int key) {
	switch (key) {
	case GLFW_KEY_SPACE : {
		nextTurn();	//Move turn on when space pressed
		break;
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
	map.drawMap();
	ui.drawUI();
}

void Game::nextTurn() {
	ui.changeText(gameUI::Text::ROUND, ++turnNo);	//increment turn
	for (unsigned int i = 0; i < territories.size(); i++) 
		territories[i].resetTroops();
	handleMouseInput(0, 0, false, true);
}

void Game::updateScore(int player) {

}