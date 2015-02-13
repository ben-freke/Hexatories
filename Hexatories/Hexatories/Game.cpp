#include <vector>
#include "Game.h"
#include "Shaders.h"
#include "Textures.h"
#include "log.h"

using namespace std;

bool Game::initGame(Territory *territories) {

	if (!map.initMap(territories))
		return false;

	return true;
}

void Game::draw() {
	map.drawMap();
}