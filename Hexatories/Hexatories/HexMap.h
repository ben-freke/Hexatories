#include <GL\glew.h>
#include <GLFW/glfw3.h>

#ifndef __HEXMAP_H
#define __HEXMAP_H

	class HexTile {
		GLuint vaoid, vboid;
		int x, y;
	public:
		void initTile(int, int);
		void drawTile();
	};

	class HexMap {
		HexTile myHex[52][41];
	public:
		HexMap();
		void drawMap();
	};

#endif