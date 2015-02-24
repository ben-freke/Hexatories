#include <vector>
#include <GL\glew.h>
#include <GLFW/glfw3.h>

#ifndef __MAINMENU_H
#define __MAINMENU_H

class MainMenu {

	static const int sectionCoords[];	// All the areas on screen co ords (see Section enum)

#pragma region openglStuff

	GLuint vao, vbo, prog;
	GLint uniform;

	std::vector<GLint> verts;	//All vertices

	/*
	Sets up our VAO and loads all textures needed
	*/
	void setupVAO();

	/*
	Updates our VAO with new VBO
	*/
	void updateVAO();

public:

	enum class Section {
		TUTORIAL = 0, NEW_GAME, LOAD_GAME, EXIT,
		NULL_SEC,
	};	//Different areas requiring mouse input

	void initMenu();

	void drawMenu();

	/*
		Changes the menu screen
	*/
	void changeScreen(int);

	/*
	Gets the section a point is in
	*/
	Section pointInBox(int, int);

};

#endif