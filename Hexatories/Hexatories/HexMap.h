#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <string>

#ifndef __HEXMAP_H
#define __HEXMAP_H

	/*
		Currently only contains function to calculate the positions of a certain hex tiles vertices.
	*/
	class HexTile {
	public:
		/*
			Calculate vertex position. Also includes the colour of each vertex.
			Usage:
			HexTile.initTile(xPos, yPos, vertexArray, currentPositionInVertexArray, tileColourType);
			It appends the tiles vertices to the end of the passed array.
		*/
		void initTile(int, int, GLfloat *, int, int);
	};

	/*
		Currently contains functionality to generate the entire vbo of the hexmap and draw it.
		Also allows user designed maps to be read from file.
	*/
	class HexMap {
		/*
			vaoCol - attribute array for the coloured tiles.

			progCol - the program containing the various required shaders for the tiles.
			progTex - shaders for black wireframe overlay texture
		*/
		GLuint vaoCol, vaoTex, progCol, progTex;
		/*
			Returns the string containing map data to interpret. Really needs reworking.
			Usage: string map = mapFromFile("BeachMap");
		*/
		std::string mapFromFile(const char *);
	public:
		/*
			Calculate all the vertices of the hexmap, set up the vao and link the program.
		*/
		HexMap();
		/*
			Just draws the map currently.
		*/
		void drawMap();
		/*
			Converts mouse point to x and y of tile in the map.
			Return true if in the grid.
		*/
		static bool pointToTile(double, double, int &, int &);
	};

#endif