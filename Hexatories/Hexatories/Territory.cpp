#include <algorithm>
#include <iostream>
#include "Territory.h"
#include "log.h"

using namespace std;
#pragma region otherFuncs
void Territory::initTerritory(vector<tile_t> innerTiles, int tileCount, int own, int terrNum) {

	if (own != 0) {
		attackers[0] = 1;
		defenders[0] = 1;
	}

	terrNo = terrNum;
	size = tileCount;
	owner = own;
	colour = own;
	population = 10;
	setupTiles(innerTiles);
}

void Territory::initTerritory(int own, std::vector<tile_t> borders, std::vector<tile_t> inner, int attack[2], 
	int defence[2], int pop, int terrNum) {
	
	size = borders.size() + inner.size();

	terrNo = terrNum;
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

int Territory::getTerrNo() {
	return terrNo;
}

void Territory::setOwner(int newOwner) {
	owner = newOwner;
}

void Territory::getInfo(int *vals) {
	vals[0] = population;	//pop
	vals[1] = attackers[0];	//attack
	vals[2] = defenders[0];	//def
	vals[3] = attackers[0] + attackers[1];	//total atk
	vals[4] = defenders[0] + defenders[1];	//total def
}

int Territory::getScore() {
	return population + 2 * (attackers[0] + attackers[1] + defenders[0] + defenders[1]) + size;	//p + 2(d + a) + t
}

void Territory::setupTiles(vector<tile_t> innerTiles) {

	int x, y, xoff, adjacentTiles;
	int xChange[6] = { 0, 0, 1, 1, -1, -1 };	//relative coords for surrounding tiles
	int yChange[6] = { -1, 1, -1, 0, -1, 0 };

	for (int i = 0; i < size; i++) {

		x = innerTiles[i].x;	//gets the x/y position of the tile we are checking is border or not
		y = innerTiles[i].y;
		xoff = x % 2;	//x offset (every other column is lower by 1/2 and therefore has different surrounding tiles
		adjacentTiles = 0;	//to count how many adjacent tiles are in the same territory

		for (int j = 0; j < 6; j++) {	//Loops through surrounding tiles
			if (find_if(innerTiles.begin(), innerTiles.end(),	//if finds the tile it is checking in the vector that contains our tiles
				findTile(x + xChange[j], y + yChange[j] + xoff * abs(xChange[j]))) != innerTiles.end()) {
				adjacentTiles++;	//add one to the number of tiles we are adjacent to (in our territory)
			}
		}

		if (adjacentTiles == 6) {	//if we have 6 adjacent tiles in the same territory (all of them) we are an inner tile
			tiles.push_back(innerTiles[i]); 
		} else {	//else we have a border with a different territory
			border.push_back(innerTiles[i]);
		}
	}
}

void Territory::findNeighbours(vector<tile_t> allTiles) {
	int x, y, xoff;
	int xChange[6] = { 0, 0, 1, 1, -1, -1 };	//relative coords for surrounding tiles
	int yChange[6] = { -1, 1, -1, 0, -1, 0 };

	for (unsigned int i = 0; i < border.size(); i++) {

		x = border[i].x;	//gets the x/y position of the tile we are checking
		y = border[i].y;
		xoff = x % 2;	//x offset (every other column is lower by 1/2 and therefore has different surrounding tiles

		vector<tile_t>::iterator it;
		for (int j = 0; j < 6; j++) {	//Loops through surrounding tiles

			if ((it = find_if(allTiles.begin(), allTiles.end(),	// finds the tile's neighbours
				findTile(x + xChange[j], y + yChange[j] + xoff * abs(xChange[j])))) != allTiles.end()) {

				if (border[i].terrNo != it->terrNo)	//If different owners

					if (find(neighbours.begin(), neighbours.end(), it->terrNo) == neighbours.end())	//and not already registered neighbour
						neighbours.push_back(it->terrNo);
			}
		}

	}
}

bool Territory::isNeighbour(Territory terr) {
	return (find(neighbours.begin(), neighbours.end(), terr.getTerrNo()) != neighbours.end());
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
bool  Territory::addAttacker() {
	if (population <= 10) return false;
	attackers[1]++;
	population--;
	return true;
}

bool Territory::addDefender() {
	if (population <= 10) return false;
	defenders[1]++;
	population--;
	return true;
}

int Territory::getAttackers() {
	return attackers[0];
}

int Territory::getDefenders() {
	return defenders[0];
}

int Territory::getDefense() {
	return defenders[0] * 15 + (population - 10);
}

void Territory::destroyAttackers() {
	//If we are calling this it is from another territory, we must have taken over this territory thus destroy all
	attackers[0] = 0;
	attackers[1] = 0;
}

void Territory::destroyDefenders(int i) {
	if (i == -1) {	//If destroy all (in case this terr was attacked and lost)
		defenders[0] = 0;
		defenders[1] = 0;
		population = 10;
		return;
	}	//if we need to get rid of more defenders than we have (due to population defense)
	if (i > defenders[0]) {
		i -= defenders[0];
		defenders[0] = 0;
		population -= i * 15;	//Remove needed population
		return;
	}
	defenders[0] -= i;

}

bool Territory::sendTroops(Territory &receivingTerr, int noAttack, int noDefend) {

	/*
	Troop type can be either 1 or 0, where 1 is attackers and 0 is defenders.
	*/
	if (receivingTerr.getOwner() == owner) {
		if (attackers[0] < noAttack || defenders[0] < noDefend) return false;	//If we don't have enough troops
		attackers[0] -= noAttack;
		defenders[0] -= noDefend;
		receivingTerr.receiveTroops(noAttack, noDefend);
		return false;
	} else {

		if (noDefend != 0) return false;
		if (noAttack == 0) return false;

		if (attackers[0] >= noAttack) {
			
			int attack = noAttack * 15;
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
				receivingTerr.destroyDefenders(noAttack);
				attackers[0] -= noAttack;	//all attackers sent are destroyed
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

				attackers[0] -= noAttack;	//Destroy lost troops
				receivingTerr.receiveTroops(remainingTroops, 0);

				receivingTerr.setOwner(owner);

				cout << "Success in attacking\n";
			}

		} else {
			cout << "You do not have enough troops to send\n";
			return false;
		}
	} 
	return true;
}

void Territory::incrementTurn(int &coins) {
	attackers[0] += attackers[1];	//add used attackers to unused
	attackers[1] = 0;	//clear unused

	defenders[0] += defenders[1];
	defenders[1] = 0;

	if (coins != -1)	//	Add taxed coins
		coins += (int) (population * ((bankBuilt > -1) ? 1.1 : 1));

	population = (int)(population * (1.1 + ((farmBuilt > -1) ? .1 : 0)));
	if (population > size * 10) population = size * 10;

}

int Territory::getPopulation() {
	return population;
}

void Territory::receiveTroops(int numAtk, int numDef) {
	attackers[1] += numAtk;
	defenders[1] += numDef;
}
#pragma endregion

#pragma region openglStuffs
array<GLint, 32> Territory::getTileRect(int x, int y, int z) {

	array<GLint, 32> verts = {	//Default verts, z is a texture number for the shader
		147, 736, 0, 0, 0, 0, 0, z,
		873, 736, 0, 0, 0, 1, 0, z,
		873, 0, 0, 0, 0, 1, 1, z,
		147, 0, 0, 0, 0, 0, 1, z,
	};

	int xoff = x % 2;

	verts[0] += 18 * x;	//calculates the vertices based on the x/y of the tile
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

	vboPos = vertices.size() + 7;	//+7 as we will only want to change the colour which will start 7 in from the current end
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
	for (unsigned int i = 0; i < border.size(); i++) {	//loops through all borders changing the colour in the vector
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

	int building = -1, tile;	//building = building type, -1 null 0 farm, 1 bank
	array<GLint, 32> verts;

	if (type == true && farmBuilt == -1) {	//if we want to build a bank and don't have a bank
		do {
			farmBuilt = rand() % tiles.size();	//choose random tile making sure bank isn't already on it
		} while (farmBuilt == bankBuilt);

		tile = farmBuilt;
		building = 0;
	}
	if (type == false && bankBuilt == -1) {	//same as farm, but for banks

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