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

			if (prevTile.x != -1) {
				territories[prevTile.terrNo].setColour(-1);
				map.updateVBO(territories[prevTile.terrNo]);
			}

			prevTile = currTile;
			territories[prevTile.terrNo].setColour(3);
			map.updateVBO(territories[prevTile.terrNo]);
		}
	}
}

void Game::draw() {
	map.drawMap();
}