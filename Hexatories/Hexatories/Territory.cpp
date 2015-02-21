#include <algorithm>
#include "Territory.h"
#include "log.h"

using namespace std;

void Territory::initTerritory(vector<tile_t> innerTiles, int tileCount, int own) {
	size = tileCount;
	owner = own;
	colour = own;
	setupTiles(innerTiles);
}

void Territory::setupTiles(vector<tile_t> innerTiles) {

	int x, y, xoff, adjacentTiles;
	int xChange[6] = { 0, 0, 1, 1, -1, -1 };
	int yChange[6] = { -1, 1, -1, 0, -1, 0 };

	for (int i = 0; i < size; i++) {

		x = innerTiles[i].x;
		y = innerTiles[i].y;
		xoff = x % 2;
		adjacentTiles = 0;

		for (int j = 0; j < 6; j++) {
			if (find_if(innerTiles.begin(), innerTiles.end(), findTile(x + xChange[j], y + yChange[j] + xoff * abs(xChange[j]))) != innerTiles.end()) {
				adjacentTiles++;
			}
		}

		if (adjacentTiles == 6) {
			tiles.push_back(innerTiles[i]); 
		} else {
			border.push_back(innerTiles[i]);
		}
	}
}

bool Territory::isSelected() {
	return selected;
}

void Territory::invSelect() {
	selected = !selected;
}

void Territory::setColour(int newCol) {
	if (newCol != -1) {
		colour = newCol;
	} else {
		colour = owner;
	}
}

array<GLint, 32> Territory::getTileRect(int x, int y, int z) {

	array<GLint, 32> verts = {
		147, 736, 0, 0, 0, 0, 0, z,
		873, 736, 0, 0, 0, 1, 0, z,
		873, 0, 0, 0, 0, 1, 1, z,
		147, 0, 0, 0, 0, 0, 1, z,
	};

	int xoff = x % 2;

	verts[0] += 18 * x;
	verts[24] = verts[0];
	verts[8] = verts[0] + 24;
	verts[16] = verts[8];

	verts[1] -= (22 * y) + (11 * xoff);
	verts[9] = verts[1];
	verts[17] = verts[1] - 23;
	verts[25] = verts[17];

	return verts;
}

void Territory::addBuilding(bool type, vector<GLint> &vertices, vector<GLushort> &indices) {

	GLushort baseIndex = vertices.size() / 8;
	GLushort rectIndices[] = {
		0, 1, 2,
		2, 3, 0,
	};

	int building = 0;
	array<GLint, 32> verts;

	if (type == true && !(farmBuilt)) {
		farmBuilt = true;
		building = 5;
	}
	if (type == false && !(bankBuilt)) {
		bankBuilt = true;
		building = 6;
	}
	if (building != 0) {
		verts = getTileRect(tiles[0].x, tiles[1].y, building);

		for (int j = 0; j < 32; j++) {
			vertices.push_back(verts[j]);
		}

		for (int j = 0; j < 6; j++) {
			indices.push_back(baseIndex + rectIndices[j]);
		}

	}
}

void Territory::getBorderVBO(vector<GLint> &vertices, vector<GLushort> &indices) {

	GLushort rectIndices[] = {
		0, 1, 2,
		2, 3, 0,
	};

	vboPos = vertices.size() + 7;
	int baseIndex = (vertices.size() / 8);

	for (unsigned int i = 0; i < border.size(); i++) {

		array<GLint, 32> verts = getTileRect(border[i].x, border[i].y, colour);

		for (int j = 0; j < 32; j++) {
			vertices.push_back(verts[j]);
		}

		for (int j = 0; j < 6; j++) {
			indices.push_back(baseIndex + rectIndices[j] + 4 * i);
		}
	}

}

void Territory::updateBorderVBO(vector<GLint> &verts) {
	for (unsigned int i = 0; i < border.size(); i++) {
		for (int j = 0; j < 4; j++)
			verts[vboPos + (32 * i) + (8 * j)] = colour;
	}
}

int Territory::getOwner(){
	return owner;
}

void Territory::setOwner(int newOwner){
	owner = newOwner;
}

/*
	Attack and Defence Functions
*/

int Territory::getAttackers(){
	return attackers[0];
}

int Territory::getDefense(){
	return defenders[0] * 15 + population;
}

void Territory::addAttacker(){
	attackers[0]++;
}

/*
	TODO: Implement addDefender()
*/

bool Territory::destroyAttackers(int i){
	if (i == -1) {
		attackers[0] = 0; 
		return true;
	}
	if (i > attackers[0]) return false;
	attackers[0] -= i;
	return true;
}

bool Territory::destroyDefenders(int i){
	if (i == -1) {
		defenders[0] = 0;
		return true;
	}
	if (i > defenders[0]) return false;
	defenders[0] -= i;
	return true;
}

/*
	TODO: Implement changeDefender()
*/

bool Territory::sendTroops(Territory &targetTerr, int noAtk, int noDef){
	if (attackers[0] < noAtk || defenders[0] < noDef) return false;
	attackers[0] -= noAtk;
	defenders[0] -= noDef;
	targetTerr.receiveTroops(noAtk, noDef);
	return true;
}

void Territory::reset() {
	attackers[0] += attackers[1];
	attackers[1] = 0;

	defenders[0] += defenders[1];
	defenders[1] = 0;
}

void Territory::getInfo(int *vals) {
	vals[0] = population;
	vals[1] = attackers[0] + attackers[1];
	vals[2] = defenders[0] + defenders[1];
}

/*
	TODO: Implement sendDefender()
*/

void Territory::receiveTroops(int numAtk, int numDef){
	attackers[1] += numAtk;
	defenders[1] += numDef;
}

/*
	TODO: Implement receiveDefender()
*/


bool operator==(const tile_t &tile1, const tile_t &tile2) {
	return ((tile1.x == tile2.x) && (tile1.y == tile2.y));
}