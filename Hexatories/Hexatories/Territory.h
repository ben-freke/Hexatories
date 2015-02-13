#include <vector>
#include <GL\glew.h>
#include <GLFW/glfw3.h>

#ifndef __TERRITORY_H
#define __TERRITORY_H

struct tile_t {
	int x, y;
};

struct findTile {

	int m_x, m_y;

	findTile(int const &x, int const &y) : m_x(x), m_y(y) {}

	bool operator()(tile_t const &tile) const {
		return tile.x == m_x && tile.y == m_y;
	}
};

class Territory {

	std::vector<tile_t> tiles, border;
	int size;

	void setupTiles(std::vector<tile_t>);

public:

	void initTerritory(std::vector<tile_t>, int);

	void getBorderVBO();

};
#endif