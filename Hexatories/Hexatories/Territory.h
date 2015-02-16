#include <vector>
#include <GL\glew.h>
#include <GLFW/glfw3.h>

#ifndef __TERRITORY_H
#define __TERRITORY_H

struct Attacker
{
	bool moveFlag = false;
};

struct Defender
{
	bool moveFlag = false;
};

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
	std::vector<Attacker> attackers;
	std::vector<Defender> defenders;


	int populationNo, troopsAttack, troopsDefense, size, owner, vboPos, colour;
	bool farmBuilt, bankBuilt = false;

	void setupTiles(std::vector<tile_t>);

public:

	void initTerritory(std::vector<tile_t>, int, int);
	
	void setColour(int);

	int getOwner();

	void setOwner(int newOwner);

	/*
		Gets the number of attackers and defenders.
	*/

	int getNoAttackers();

	int getNoDefenders();

	/*
		Adds attackers and defenders
		TODO: Implement addDefender()
	*/

	void addAttacker();

	/*
		Destroys attackers or defenders
	*/

	void destroyAttacker(int i);

	void destroyDefender(int i);

	/*
		Changes the state of attackers or defedners (e.g. moved in turn = true)
		TODO: Implement changeDefender()
	*/

	void changeAttacker(int id, bool state);

	/*
		Sends attackers and defenders
		TODO: Implement sendDefender()
	*/

	void sendAttacker(Territory &targetTerr, int noTroops);

	/*
		Receives attackers and defenders
		TODO: Implement receiveDefender()
	*/
	
	void receiveAttacker(Attacker troop);

	
	void setupBorderTiles();

	void getBorderVBO(std::vector<GLint> &, std::vector<GLushort> &);

	void updateBorderVBO(std::vector<GLint> &);

};
#endif