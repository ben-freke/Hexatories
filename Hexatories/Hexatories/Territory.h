#include <vector>
#include <array>
#include <GL\glew.h>
#include <GLFW/glfw3.h>

#ifndef __TERRITORY_H
#define __TERRITORY_H

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

class Territory {

	std::vector<tile_t> tiles, border;
	
	int attackers[2];
	int defenders[2];

	int population, troopsAttack, troopsDefense, size, owner, vboPos, colour;
	bool farmBuilt = false, bankBuilt = false, selected = false;

	void setupTiles(std::vector<tile_t>);

public:

	void initTerritory(std::vector<tile_t>, int, int);

	std::array<GLint, 32> getTileRect(int, int, int);

	void addBuilding(bool, std::vector<GLint> &, std::vector<GLushort> &);
	
	void setColour(int);

	int getOwner();

	void setOwner(int newOwner);

	void reset();

	/*
		Gets the number of attackers and defenders.
	*/

	int getAttackers();

	int getDefense();

	/*
		Adds attackers and defenders
		TODO: Implement addDefender()
	*/

	void addAttacker();

	/*
		Destroys attackers or defenders
	*/

	bool destroyAttackers(int i);

	bool destroyDefenders(int i);

	/*
		Sends attackers and defenders
		TODO: Implement sendDefender()
	*/

	bool sendTroops(Territory &targetTerr, int numAtk, int numDef);

	/*
		Receives attackers and defenders
		TODO: Implement receiveDefender()
	*/
	
	void receiveTroops(int numAtk, int numDef);

	
	void setupBorderTiles();

	void getBorderVBO(std::vector<GLint> &, std::vector<GLushort> &);

	void updateBorderVBO(std::vector<GLint> &);

	bool isSelected();

	void invSelect();

	void getInfo(int *);
};
#endif