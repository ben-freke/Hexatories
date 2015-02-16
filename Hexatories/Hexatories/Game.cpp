#include "Game.h"
#include "Shaders.h"
#include "Textures.h"
#include "log.h"

using namespace std;

bool Game::initGame() {

	if (!map.initMap(territories))
		return false;

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

int Game::getTerritory(double x, double y){
	tile_t currTile = map.pointToTile(x, y);
	return currTile.terrNo;
}

void Game::changeTerritoryColour(Territory ter, int col) {
	ter.setColour(col);
	map.updateVBO(ter);
}

void Game::draw() {
	map.drawMap();
}