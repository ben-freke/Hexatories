#include <algorithm>
#include <iostream>
#include "Territory.h"
#include "log.h"

using namespace std;
#pragma region otherFuncs
void Territory::initTerritory(vector<tile_t> innerTiles, int tileCount, int own) {
	size = tileCount;
	owner = own;
	colour = own;
	population = 10;
	setupTiles(innerTiles);
}

void Territory::initTerritory(int own, std::vector<tile_t> borders, std::vector<tile_t> inner, int attack[2], 
	int defence[2], int pop) {

	owner = own;
	colour = own;
	border = borders;
	tiles = inner;

	for (int i = 0; i < 2; i++) {
		attackers[i] = attack[i];
		defenders[i] = defence[i];
	}

	population = pop;

}

int Territory::getOwner() {
	return owner;
}

void Territory::setOwner(int newOwner) {
	owner = newOwner;
}

void Territory::getInfo(int *vals) {
	vals[0] = population;
	vals[1] = attackers[0];
	vals[2] = defenders[0];
	vals[3] = attackers[0] + attackers[1];
	vals[4] = defenders[0] + defenders[1];
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
			if (find_if(innerTiles.begin(), innerTiles.end(), 
				findTile(x + xChange[j], y + yChange[j] + xoff * abs(xChange[j]))) != innerTiles.end()) {
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

void Territory::fillSaveData(string &saveData) {

	saveData.append(to_string(owner));
	saveData.append(",");

	saveData.append(to_string(border.size()));		// border tiles
	saveData.append(",");	

	for (unsigned int i = 0; i < border.size(); i++) {
		saveData.append(to_string(border[i].x));
		saveData.append(",");
		saveData.append(to_string(border[i].y));
		saveData.append(",");
	}

	saveData.append(to_string(tiles.size()));		// inner tiles
	saveData.append(",");

	for (unsigned int i = 0; i < tiles.size(); i++) {
		saveData.append(to_string(tiles[i].x));
		saveData.append(",");
		saveData.append(to_string(tiles[i].y));
		saveData.append(",");
	}

	for (int i = 0; i < 2; i++) {					// attackers/defenders
		saveData.append(to_string(attackers[i]));
		saveData.append(",");
		saveData.append(to_string(defenders[i]));
		saveData.append(",");
	}

	saveData.append(to_string(population));			// population
	saveData.append(",");

	saveData.append(to_string(farmBuilt));			// farm
	saveData.append(",");

	saveData.append(to_string(bankBuilt));			// bank
	saveData.append(",");
}
#pragma endregion
 
#pragma region troops
void Territory::addAttacker() {
	attackers[1]++;
}

void Territory::addDefender() {
	defenders[1]++;
}

int Territory::getAttackers() {
	return attackers[0];
}

int Territory::getDefense() {
	return defenders[0] * 15 + (population - 10);
}

void Territory::destroyAttackers() {
	attackers[0] = 0;
	attackers[1] = 0;
}

void Territory::destroyDefenders(int i) {
	if (i == -1) {
		defenders[0] = 0;
		defenders[1] = 0;
		population = 10;
		return;
	}
	if (i > defenders[0]) {
		i -= defenders[0];
		defenders[0] = 0;
		population -= i * 15;
		return;
	}
	defenders[0] -= i;

}

bool Territory::sendTroops(Territory &targetTerr, int noAtk, int noDef) {
	if (attackers[0] < noAtk || defenders[0] < noDef) return false;
	attackers[0] -= noAtk;
	defenders[0] -= noDef;
	targetTerr.receiveTroops(noAtk, noDef);
	return true;
}

void Territory::sendTroopsMoved(Territory &receivingTerr, int troopType, int noTroops) {

	/*
	Troop type can be either 1 or 0, where 1 is attackers and 0 is defenders.
	*/
	if (troopType == 0) {

		//Send Defender


	} else if (troopType == 1) {
		if (attackers[0] >= noTroops) {
			if (receivingTerr.getOwner() != owner) {

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
					Destroys any lost defenders in the territory
					*/
					receivingTerr.destroyDefenders(noTroops);
					attackers[0] -= noTroops;	//all attackers sent are destroyed
				}
				/*
				Attack has won. All defenders should be destroyed and remaining attackers recorded.
				*/
				else {
					remainingTroops = remainingTroops * -1;

					/*
					Destroys any attackers/defenders remaining in the territory
					*/
					receivingTerr.destroyDefenders(-1);
					receivingTerr.destroyAttackers();

					attackers[0] -= noTroops - remainingTroops;	//Destroy lost troops

					sendTroops(receivingTerr, remainingTroops, 0);	//send remaining troops

					receivingTerr.setOwner(owner);

					cout << "Success in attacking\n";
				}
			} else {
				cout << "You have the same owner\n";
			}
		} else {
			cout << "You do not have enough troops to send\n";
		}
	}
}

void Territory::resetTroops() {
	attackers[0] += attackers[1];
	attackers[1] = 0;

	defenders[0] += defenders[1];
	defenders[1] = 0;
}

void Territory::receiveTroops(int numAtk, int numDef) {
	attackers[1] += numAtk;
	defenders[1] += numDef;
}
#pragma endregion

#pragma region openglStuffs
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

void Territory::addBuilding(bool type, vector<GLint> &vertices, vector<GLushort> &indices) {

	GLushort baseIndex = vertices.size() / 8;
	GLushort rectIndices[] = {
		0, 1, 2,
		2, 3, 0,
	};

	int building = -1, tile;
	array<GLint, 32> verts;

	if (type == true && farmBuilt == -1) {
		do {
			farmBuilt = rand() % tiles.size();
		} while (farmBuilt == bankBuilt);

		tile = farmBuilt;
		building = 0;
	}
	if (type == false && bankBuilt == -1) {

		do {
			bankBuilt = rand() % tiles.size();
		} while (farmBuilt == bankBuilt);

		tile = bankBuilt;
		building = 1;
	}

	if (building != -1) {
		verts = getTileRect(tiles[tile].x, tiles[tile].y, building + 5);

		for (int j = 0; j < 32; j++) {
			vertices.push_back(verts[j]);
		}

		for (int j = 0; j < 6; j++) {
			indices.push_back(baseIndex + rectIndices[j]);
		}

	}
}

void Territory::setColour(int newCol) {
	if (newCol != -1) {
		colour = newCol;
	} else {
		colour = owner;
	}
}

bool Territory::isSelected() {
	return selected;
}

void Territory::invSelect() {
	selected = !selected;
}
#pragma endregion

bool operator==(const tile_t &tile1, const tile_t &tile2) {
	return ((tile1.x == tile2.x) && (tile1.y == tile2.y));
}