#include <algorithm>
#include <array>
#include "Territory.h"
#include "log.h"

using namespace std;

void Territory::initTerritory(vector<tile_t> innerTiles, int tileCount, int own) {

	size = tileCount;
	owner = own;
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

void Territory::getBorderVBO(vector<GLint> &vertices, vector<GLushort> &indices) {

	array<GLint, 32> defVerts = {
		152, 740, 0, 0, 0, 0, 0, owner,
		878, 740, 0, 0, 0, 1, 0, owner,
		878, 0, 0, 0, 0, 1, 1, owner,
		152, 0, 0, 0, 0, 0, 1, owner,
	};

	GLushort rectIndices[] = {
		0, 1, 2,
		2, 3, 0,
	};

	int baseIndex = (vertices.size() / 8);

	int x, y, xoff;

	for (unsigned int i = 0; i < border.size(); i++) {

		array<GLint, 32> verts = defVerts;

		x = border[i].x;
		y = border[i].y;
		xoff = x % 2;

		verts[0] += 18 * x;
		verts[24] = verts[0];
		verts[8] = verts[0] + 24;
		verts[16] = verts[8];

		verts[1] -= (22 * y) + (11 * xoff);
		verts[9] = verts[1];
		verts[17] = verts[1] - 23;
		verts[25] = verts[17];

		for (int j = 0; j < 32; j++) {
			vertices.push_back(verts[j]);
		}

		for (int j = 0; j < 6; j++) {
			indices.push_back(baseIndex + rectIndices[j] + 4 * i);
		}
	}

}

int Territory::getOwner(){
	return owner;
}

void Territory::setOwner(int newOwner){
	owner = newOwner;
}

int Territory::getAttackScore(){
	return troopsAttack;
}

void Territory::setAttackScore(int value){
	troopsAttack = troopsAttack + value;
}

void Territory::resetAttack(){
	troopsAttack = 0;
}

int Territory::getDefenseScore(){
	return troopsDefense;
}

void Territory::setDefenseScore(int value){
	troopsDefense = troopsDefense + value;
}

void Territory::resetDefense(){
	troopsDefense = 1;
}