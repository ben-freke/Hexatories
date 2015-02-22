#include <vector>
#include <array>
#include <string>
#include <GL\glew.h>
#include <GLFW/glfw3.h>

#ifndef __TERRITORY_H
#define __TERRITORY_H

#pragma region tileFuncs
struct tile_t {
	int x, y, terrNo;
};

struct findTile {

	int m_x, m_y;

	findTile(int const &x, int const &y) : m_x(x), m_y(y) {}

	bool operator()(tile_t const &tile) const {
		return tile.x == m_x && tile.y == m_y;
	}
};

bool operator==(const tile_t &tile1, const tile_t &tile2);
#pragma endregion

class Territory {

	std::vector<tile_t> tiles, border;
	
	int attackers[2];
	int defenders[2];

	int population, size, owner, farmBuilt = -1, bankBuilt = -1;
	int colour, vboPos;

	bool selected = false;

	
#pragma region troops
	int getAttackers();

	int getDefense();

	void destroyAttackers();

	void destroyDefenders(int i);

	void receiveTroops(int numAtk, int numDef);
#pragma endregion

	void setupTiles(std::vector<tile_t>);

	std::array<GLint, 32> getTileRect(int, int, int);

public:

#pragma region otherFuncs
	void initTerritory(std::vector<tile_t>, int, int);
	void initTerritory(int, std::vector<tile_t>, std::vector<tile_t>, int[], int[], int);

	int getOwner();

	void setOwner(int newOwner);

	void getInfo(int *);

	void fillSaveData(std::string &);
#pragma endregion

#pragma region troops
	void addAttacker();

	void addDefender();

	bool sendTroops(Territory &targetTerr, int numAtk, int numDef);

	void sendTroopsMoved(Territory &, int, int);

	void resetTroops();
#pragma endregion

#pragma region territoryDrawMethods
	void getBorderVBO(std::vector<GLint> &, std::vector<GLushort> &);

	void updateBorderVBO(std::vector<GLint> &);

	void addBuilding(bool, std::vector<GLint> &, std::vector<GLushort> &);

	void setColour(int);

	bool isSelected();

	void invSelect();
#pragma endregion
};
#endif