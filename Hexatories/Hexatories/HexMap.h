#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <string>

#ifndef __HEXMAP_H
#define __HEXMAP_H

	class HexTile {
	public:
		void initTile(int, int, GLfloat *, int, int);
	};

	class HexMap {
		GLuint vao, vbo, ebo, progCol;
		GLfloat verts[63960];
		short indices[25584];
		HexTile myHex;
		std::string mapFromFile(const char *path);
	public:
		HexMap();
		void drawMap();
	};

#endif