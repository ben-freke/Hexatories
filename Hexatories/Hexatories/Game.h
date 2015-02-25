#include <vector>
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "Territory.h"
#include "HexMap.h"
#include "gameUI.h"
#include "MainMenu.h"
#include "Audio.h"

#ifndef __GAME_H
#define __GAME_H

/*
	Contains necessary information to remember about a player
*/
struct player {
	int coins, score, population;
};

/*
	Runs everything about the game in one way or another
*/
class Game {

	int turnNo = 1, stage = 0;	//stage is for main menu/game management

	player players[2]; // 0 = AI 1 = you

	HexMap map;
	gameUI ui;
	MainMenu mm;

	Audio gameMusic;
	Audio clickSound;
	Audio marchSound;
	Audio swordClang;
	Audio victorySound;

	std::vector<Territory> territories;	//All the territories on the current map

	/*
		Increment turn, resetting territories attackers/defenders and incrementing Population/coins
	*/
	void nextTurn();

	/*
		Loops through and updates the score for the player selected (passed int)
	*/
	void updateScore(int);

	/*
		Displays player info on screen
	*/
	void updatePlayerInfo();

#pragma region territoryHandling
	/*
		Returns the territory under the mouse co ordinates
	*/
	Territory *getTerritory(int, int);

	/*
		Selects a territory, updating the information on the sidebar and highlighting it until it is deselected
	*/
	void selectTerr(Territory *, Territory *);

	void updateTerrInfo(Territory *);

	/*
		Makes a territories border gold (used on mouseover)
	*/
	void highlightTerritory(Territory *);

	/*
		Changes a territories border colour.
	*/
	void changeTerritoryColour(Territory &ter, int col);
#pragma endregion

#pragma region saving
	/*
		Loads save from file
	*/
	std::vector<int> Game::getSave();

	/*
		Starts a new game, allowing the map to initialise the territories from the map it loads
	*/
	void newGame();

	/*
		Loads map from file, sets up territories and players
	*/
	bool loadGame();

	/*
		Saves the game to file
	*/
	void saveGame();
#pragma endregion

	bool handleMainMenu(int, int);

public:

	/*
		Initialises the game
	*/
	void initGame();

	/*
		Batch draw function
	*/
	void draw();

	/*
		Handle all mouse click/mouse over events
	*/
	bool handleMouseInput(double, double, bool, bool);

	/*
		Handles all keyboard input
	*/
	void handleKeyInput(int);
};

#endif