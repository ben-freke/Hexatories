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
};	//Used in arrays to store tiles in a territory

/*
	Compares two tiles, used for find_if
*/
struct findTile {

	int m_x, m_y;

	findTile(int const &x, int const &y) : m_x(x), m_y(y) {}

	bool operator()(tile_t const &tile) const {
		return tile.x == m_x && tile.y == m_y;
	}
};

/*
	Allows use of the == operator on tiles
*/
bool operator==(const tile_t &tile1, const tile_t &tile2);
#pragma endregion

class Territory {

	std::vector<tile_t> tiles, border;
	
	int attackers[2];	//[0] unused [1] used
	int defenders[2];

	int population, size, owner, farmBuilt = -1, bankBuilt = -1;
	int colour, vboPos;

	bool selected = false;	//Used to determine if we should stay highlighted when mouse is off us

	
#pragma region troops

	/*
		Gets the defense of a territory (15 * defenders[1] + pop)
	*/
	int getDefense();

	/*
		Destroys all attackers on a tile (will never need to be x amount)
	*/
	void destroyAttackers();

	/*
		Destroys i defenders (plus however many population necessary)
	*/
	void destroyDefenders(int i);

	/*
		Adds troops from another territory
	*/
	void receiveTroops(int numAtk, int numDef);
#pragma endregion

	/*
		Works out which tiles are borders and inner tiles
	*/
	void setupTiles(std::vector<tile_t>);

	/*
		Gets the vertices of the rectangle surrounding a tile
	*/
	std::array<GLint, 32> getTileRect(int, int, int);

public:

#pragma region otherFuncs
	/*
		Normal constructor
	*/
	void initTerritory(std::vector<tile_t>, int, int);

	/*
		Overloaded constructor for when we load a game
	*/
	void initTerritory(int, std::vector<tile_t>, std::vector<tile_t>, int[], int[], int);

	/*
		Return who owns this territory
	*/
	int getOwner();

	/*
		Changes the owner
	*/
	void setOwner(int newOwner);

	/*
		Returns information to be displayed when territory is selected
	*/
	void getInfo(int *);

	/*
		Adds necessary data to the passed string for saving
	*/
	void fillSaveData(std::string &);
#pragma endregion

#pragma region troops
	/*
		Adds a used attacker
	*/
	void addAttacker();

	/*
		Adds a used defender
	*/
	void addDefender();

	/*
		Transfers troops from one territory to another (assuming you have them)
	*/
	bool sendTroops(Territory &targetTerr, int numAtk, int numDef);

	/*
		For attacking territories - needs reworking
	*/
	void sendTroopsMoved(Territory &, int, int);

	/*
		Sets used troops to unused
	*/
	void resetTroops();
#pragma endregion

#pragma region territoryDrawMethods
	/*
		Adds the borders to the passed vectors
	*/
	void getBorderVBO(std::vector<GLint> &, std::vector<GLushort> &);

	/*
		Updates colour of the border
	*/
	void updateBorderVBO(std::vector<GLint> &);

	/*
		Adds building to random tile on map
	*/
	void addBuilding(bool, std::vector<GLint> &, std::vector<GLushort> &);

	/*
		Changes the colour var
	*/
	void setColour(int);

	/*
		Returns selected
	*/
	bool isSelected();

	/*
		Inverts selected
	*/
	void invSelect();
#pragma endregion
};
#endif