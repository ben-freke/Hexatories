#include <iostream>
#include <fstream>
#include <Windows.h>
#include "Game.h"
#include "Shaders.h"
#include "Textures.h"
#include "log.h"

using namespace std;

void Game::initGame() {

	gameMusic.playAudio("title.wav", true);
	gameMusic.fadeInAudio(3);
	swordClang.setVolume(100);
	clickSound.setVolume(50);

	mm.initMenu(0);
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
	gongSound.playAudio("gong.wav", false);

	gameMusic.playAudio("mainBackground.wav", true);
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
	ai.initAI(&territories, &players[0]);
}

bool Game::loadGame() {
	gameMusic.playAudio("mainBackground.wav", true);
	gameMusic.setVolume(50);
	vector<int> save = getSave();	// save data
	if (save[0] == -1) {
		newGame();
		return false;
	}

	territories.clear();

	map.initMap(territories, false);	//false so map doesn't init territories
	ui.initUI();
	
	vector<int> buildings[2];	//keeps the territory numbers that have [0] farms and [1] banks built
	vector<tile_t> allTiles;

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
				allTiles.push_back(newTile);
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

		territories[terrNo].initTerritory(owner, tiles[0], tiles[1], attackers, defenders, pop, terrNo);	//init territory with read info
	}

	for (unsigned int i = 0; i < territories.size(); i++) {
		map.updateBorder(territories[i], true);
		territories[i].findNeighbours(allTiles);
	}
	
	for (unsigned int i = 0; i < buildings[0].size(); i++)	// updates all territories with farms
		map.updateBuilding(&territories[buildings[0][i]], true);
	
	for (unsigned int i = 0; i < buildings[1].size(); i++)	//updates all territories with banks
		map.updateBuilding(&territories[buildings[1][i]], false);

	updatePlayerInfo();
	ui.changeText(gameUI::Text::ROUND, turnNo);

	map.addToTiles(allTiles);
	ai.initAI(&territories, &players[0]);

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
		if (firstTerr != NULL) selectTerr(NULL, firstTerr);

		currTerr = NULL;
		firstTerr = NULL;
		secondTerr = NULL;
		updateTerrInfo(NULL);

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
	if (stage != 3 && stage != 5) return false;
#pragma endregion

#pragma region gameInput
	gameUI::Section sect = ui.pointInBox(ix, iy);

	if (stage == 5 && click) {
		switch (sect) {

		case gameUI::Section::LOST_LOAD: {
			handleMouseInput(0, 0, false, true);
			stage = 4;
			mm.initMenu(2);
			loadGame();
			break;
		}

		case gameUI::Section::LOST_MAIN:
		case gameUI::Section::WON_MAIN: {
			handleMouseInput(0, 0, false, true);
			stage = 0;
			mm.initMenu(0);
			gameMusic.playAudio("title.wav", true);
			break;
		}
		}
		return false;
	}

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
			if (gameMusic.getVolume() > 0) {
				gameMusic.setVolume(0);
			} else {
				gameMusic.setVolume(50);
			}
			break;
		}

		case gameUI::Section::MUTE_EFFECTS: {
			ui.drawSettings(2);
			if (swordClang.getVolume() == 100) {
				swordClang.setVolume(0);
				clickSound.setVolume(0);
				gongSound.setVolume(0);
				coinSound.setVolume(0);
				clickSound.setVolume(0);
				marchSound.setVolume(0);
				swordClang.setVolume(0);
				victorySound.setVolume(0);
			} else {
				swordClang.setVolume(100);
				clickSound.setVolume(100);
				gongSound.setVolume(100);
				coinSound.setVolume(100);
				clickSound.setVolume(100);
				marchSound.setVolume(100);
				swordClang.setVolume(100);
				victorySound.setVolume(100);
			}
			break;
		}

		case gameUI::Section::SAVE: {
			saveAudio.playAudio("save.wav", false);
			saveGame();
			break;
		}

		case gameUI::Section::EXIT_WINDOWS: {
			return true;
		}

		case gameUI::Section::EXIT_MAIN: {
			handleMouseInput(0, 0, false, true);
			stage = 0;
			mm.initMenu(0);
			gameMusic.playAudio("title.wav", true);
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
			clickSound.playAudio("mainClick.wav", false);

			if (!sendTroopsPressed) {	//If we haven't pressed send troops (we are just selecting a territory)

				Territory *nextTerr = getTerritory(ix, iy);	//Get the clicked on territory

				if (nextTerr == currTerr) nextTerr = NULL;	//If this and previous are the same we want to deselect it
				selectTerr(nextTerr, currTerr);	//Passing NULL as the first parameter just deselects the second, if not swaps which is selected
				currTerr = nextTerr;

				updateTerrInfo(currTerr);

				if (numAtkSend > 0 || numDefSend > 0) {
					numAtkSend = 0;
					numDefSend = 0;
					ui.changeText(gameUI::Text::SEND_ATK, 0);
					ui.changeText(gameUI::Text::SEND_DEF, 0);
				}

			} else {	//Send troops pressed, we have one territory (to send from) selected, firstTerr

				if (secondTerr != NULL) selectTerr(NULL, secondTerr);	//If we have already selected a secondTerr, we are changing it, so NULL

				secondTerr = getTerritory(ix, iy);	//Get the territory we clicked on
				if (secondTerr == firstTerr) {	//If it is the one we wanted to send from

					selectTerr(NULL, firstTerr);	//Deselect it
					updateTerrInfo(NULL);
					firstTerr = NULL;	//Reset all territories
					secondTerr = NULL;
					currTerr = NULL;

					/*
						Resets the send troops button to default and resets state
					*/
					ui.changeButton(-1);
					sendTroopsPressed = false;

					if (numAtkSend > 0 || numDefSend > 0) {
						numAtkSend = 0;
						numDefSend = 0;
						ui.changeText(gameUI::Text::SEND_ATK, 0);
						ui.changeText(gameUI::Text::SEND_DEF, 0);
					}

					break;
				}

				selectTerr(secondTerr, NULL);	//If valid select the territory
				updateTerrInfo(secondTerr);
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
			clickSound.playAudio("mainClick.wav", false);
			if (currTerr == NULL && firstTerr == NULL) break;	//If no terr selected

			if (!sendTroopsPressed) {	//If the button hasn't been pressed yet
				if (currTerr->getOwner() == 2) {
					firstTerr = currTerr;	//Remember first terr selected
					currTerr = NULL;
					sendTroopsPressed = true;
					ui.changeButton(0);	//Indent button
				}

			} else if (secondTerr != NULL) {

				if (!firstTerr->isNeighbour(*secondTerr)) break;
				

				ui.changeButton(-1);	//Resets the button

				int secondTerrOriginalOwner = secondTerr->getOwner();
				if (firstTerr->sendTroops(*secondTerr, numAtkSend, numDefSend)) {	//Send troops

					if (firstTerr->getOwner() != secondTerrOriginalOwner){
						swordClang.playAudio("swords.wav", false);
						victorySound.playAudio("territoryWon.wav", false);
						checkVictory();
					}
					updatePlayerInfo();
				}

				if (numAtkSend > 0 || numDefSend > 0){
					marchSound.playAudio("mainMarch.wav", false);

				}

				selectTerr(NULL, firstTerr);	//deselect territories
				selectTerr(NULL, secondTerr);

				firstTerr = NULL;	//reset all territories
				secondTerr = NULL;

				numAtkSend = 0;	//reset troops to send
				numDefSend = 0;

				updateTerrInfo(NULL);
				ui.changeText(gameUI::Text::SEND_ATK, 0);	//update troops to send text
				ui.changeText(gameUI::Text::SEND_DEF, 0);
				sendTroopsPressed = false;
			}
			break;
		}

		case gameUI::Section::ATK_UP: {
			clickSound.playAudio("mainClick.wav", false);
			if (firstTerr == NULL) break;
			if (numAtkSend < firstTerr->getAttackers(false))
				ui.changeText(gameUI::Text::SEND_ATK, ++numAtkSend);	//Add one to the attack to send text & var
			break;
		}

		case gameUI::Section::ATK_DOWN: {
			clickSound.playAudio("mainClick.wav", false);
			if (firstTerr == NULL) break;
			if (numAtkSend > 0) {
				ui.changeText(gameUI::Text::SEND_ATK, --numAtkSend);	//Take one from the attack to send text & var
				break;
			}
			
			if (numAtkSend == 0){
				numAtkSend = firstTerr->getAttackers(false);
				ui.changeText(gameUI::Text::SEND_ATK, numAtkSend);
			}
			
			break;
		}

		case gameUI::Section::DEF_UP: {
			clickSound.playAudio("mainClick.wav", false);
			if (firstTerr == NULL) break;
			if (numDefSend < firstTerr->getDefenders())
				ui.changeText(gameUI::Text::SEND_DEF, ++numDefSend);	//Add one to the defence to send text & var
			break;
		}

		case gameUI::Section::DEF_DOWN: {
			clickSound.playAudio("mainClick.wav", false);
			if (firstTerr == NULL) break;
			if (numDefSend > 0) {
				ui.changeText(gameUI::Text::SEND_DEF, --numDefSend);	//Take one from the attack to send text & var
				break;
			}
			if (numDefSend == 0) {
				numDefSend = firstTerr->getDefenders();
				ui.changeText(gameUI::Text::SEND_DEF, numDefSend);
			}
			break;
		}

		case gameUI::Section::BUY_FARM: {
			clickSound.playAudio("mainClick.wav", false);
			if (currTerr == NULL) break;
			if (currTerr->getOwner() == 2) {
				if (players[1].coins >= 500) {
					if (map.updateBuilding(currTerr, true)) {	//Adds a farm to the territory & map
						clickSound.playAudio("coin.wav", false);
						players[1].coins = players[1].coins - 500;
						updatePlayerInfo();
					}
				}
			}
			break;
		}

		case gameUI::Section::BUY_BANK: {
			clickSound.playAudio("mainClick.wav", false);
			if (currTerr == NULL) break;
			if (currTerr->getOwner() == 2) {
				if (players[1].coins >= 500) {
					if (map.updateBuilding(currTerr, false)) {	//Adds a bank to the territory & map
						clickSound.playAudio("coin.wav", false);
						players[1].coins = players[1].coins - 500;
						updatePlayerInfo();
					}
				}
			}
			break;
		}

		case gameUI::Section::BUY_ATTACK: {
			clickSound.playAudio("mainClick.wav", false);
			if (currTerr == NULL) break;
			if (currTerr->getOwner() == 2) {
				if (players[1].coins >= 50) {
					if (currTerr->addAttacker()) {
						clickSound.playAudio("coin.wav", false);
						players[1].coins -= 50;
						updateTerrInfo(currTerr);
						updatePlayerInfo();
					}
				}
			}
			break;
		}

		case gameUI::Section::BUY_DEFENDER: {
			clickSound.playAudio("mainClick.wav", false);
			if (currTerr == NULL) break;
			if (currTerr->getOwner() == 2) {
				if (players[1].coins >= 50) {
					if (currTerr->addDefender()) {
						clickSound.playAudio("coin.wav", false);
						players[1].coins -= 50;
						updateTerrInfo(currTerr);
						updatePlayerInfo();
					}
				}
			}
			break;
		}

		case gameUI::Section::SETTINGS: {
			clickSound.playAudio("mainClick.wav", false);
			settingsOpen = true;
			ui.drawSettings(0);
			break;
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
		clickSound.playAudio("mainClick.wav", false);
		stage = 2;
		mm.changeScreen(2);
		return false;
	}

	case (MainMenu::Section::LOAD_GAME) : {
		clickSound.playAudio("mainClick.wav", false);

		stage = 4;
		mm.changeScreen(2);
		return false;
	}

	case (MainMenu::Section::TUTORIAL) : {
		clickSound.playAudio("mainClick.wav", false);

		ShellExecute(NULL, "open", "https://www.youtube.com/watch?v=BWfxBpkp2Yo", NULL, NULL, SW_SHOWNORMAL);	
		gameMusic.setVolume(0);
		break;
	}

	case (MainMenu::Section::EXIT) : {
		clickSound.playAudio("mainClick.wav", false);

		return true;
	}

	}
	return false;
}

void Game::handleKeyInput(int key) {

	if (key == GLFW_KEY_SPACE) {

		if (stage == 0) {
			clickSound.playAudio("mainClick.wav", false);
			mm.changeScreen(1);
			stage = 1;
		}
		if (stage == 3)
			nextTurn();	//Move turn on when space pressed
		if (stage == 2) {
			clickSound.playAudio("mainClick.wav", false);
			newGame();
			stage = 3;
		}
		if (stage == 4) {
			clickSound.playAudio("mainClick.wav", false);
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

	if (prevTerr != NULL) {	
		changeTerritoryColour(*prevTerr, -1);	//Default colour (that of the owner)
		prevTerr->invSelect();	//Not selected anymore
	}

	if (terr != NULL) {
		changeTerritoryColour(*terr, 3);	//Change to gold
		terr->invSelect();	//Selected now
	}
}

void Game::updateTerrInfo(Territory *terr) {

	int vals[5];
	
	if (terr == NULL) {	//If we aren't selecting a new territory
		for (int i = 0; i < 5; i++) vals[i] = 0;	//all values are 0
	} else {
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

	if (stage == 3 || stage == 5) {
		map.drawMap();
		ui.drawUI();
	} else {
		mm.drawMenu();
	}
}

void Game::checkVictory() {
	int victory = 0; //0 not finished 1 you win 2 ai win
	int playersCount = 0, AICount = 0;

	for (unsigned int i = 0; i < territories.size(); i++) {
		if (territories[i].getOwner() == 1) AICount++;
		if (territories[i].getOwner() == 2) playersCount++;
	}

	if (playersCount == 0) victory = 2;
	if (AICount == 0) victory = 1;

	if (turnNo == 150) {
		updatePlayerInfo();

		players[0].score = players[0].coins / 2;

		for (unsigned int i = 0; i < territories.size(); i++) {
			if (territories[i].getOwner() == 1) {
				players[0].population += territories[i].getPopulation();
				players[0].score += territories[i].getScore();
			}
		}

		victory = (players[0].score >= players[1].score) ? 2 : 1;
	}

	if (victory == 1) {
		ui.drawVictory(true);
	} else if (victory == 2) {
 		ui.drawVictory(false);
	}

	if (victory != 0)
		stage = 5;
}

void Game::nextTurn() {
	gongSound.playAudio("gong.wav", false);
	vector<int> *farmArray = new vector<int>, *bankArray = new vector<int>;
	vector<int> update = ai.moveAI(farmArray, bankArray);

	for (unsigned int i = 0; i < update.size(); i++)	
		changeTerritoryColour(territories[update[i]], -1);

	for (unsigned int i = 0; i < (*farmArray).size(); i++)
		map.updateBuilding(&territories[(*farmArray)[i]], true);

	for (unsigned int i = 0; i < (*bankArray).size(); i++)
		map.updateBuilding(&territories[(*bankArray)[i]], false);

	delete farmArray, bankArray;

	ui.changeText(gameUI::Text::ROUND, ++turnNo);	//increment turn

	checkVictory();
	if (stage == 5) return;

 	for (unsigned int i = 0; i < territories.size(); i++) {

		int owner, coins = -1;
		owner = territories[i].getOwner();
		owner--;

		if (owner != -1) 
			coins = players[owner].coins;
		
		if (owner != -1)
			territories[i].incrementTurn(coins);

		if (owner >= 0) {
			players[owner].coins = coins;
			if (players[owner].coins > 99999) players[owner].coins = 99999;
		}
	}

	updatePlayerInfo();

	handleMouseInput(0, 0, false, true);
}

void Game::updatePlayerInfo() {

	players[1].population = 0;
	players[1].score = players[1].coins / 2;

	for (unsigned int i = 0; i < territories.size(); i++)
		if (territories[i].getOwner() == 2) {
			players[1].population += territories[i].getPopulation();
			players[1].score += territories[i].getScore();
		}

	ui.changeText(gameUI::Text::COINS, players[1].coins);
	ui.changeText(gameUI::Text::TOTAL_POP, players[1].population);
	ui.changeText(gameUI::Text::SCORE, players[1].score);
}