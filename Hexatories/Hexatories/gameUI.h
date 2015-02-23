#include <vector>
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "Territory.h"

#ifndef __GAMEUI_H
#define __GAMEUI_H

class gameUI {
#pragma region vars
	static const int sectionCoords[];	// All the areas on screen co ords (see Section enum)

	bool firstTime = true;	//If first time gameUI has been initialised
#pragma endregion

#pragma region openglStuff

	GLint uniforms[5];	//Uniforms for openGL textures
	GLuint vao, vbo, prog;

	std::vector<GLint> verts;	//All vertices
	std::vector<GLushort> indices;	//Draw order of vertices

	bool settingsOpen = false;

	/*
		Sets up our VAO and loads all textures needed
	*/
	void setupVAO();

	/*
		Updates our VAO with new VBO/EBO
	*/
	void updateVAO();

	/*
		Adds the main GUI to screen
	*/
	void mainOverlay();

	/*
		Initialises all text on screen 
	*/
	void initText();
#pragma endregion
public:

	enum class Text {
		COINS = 0, TOTAL_POP, TOTAL_ATK, TOTAL_DEF,
		POP, ATK, DEF, SEND_ATK, SEND_DEF, SCORE, ROUND,
	};  //Different types of modifiable text on screen

	enum class Section {
		MAP = 0, SEND_TROOPS, ATK_UP, ATK_DOWN,
		DEF_UP, DEF_DOWN, BUY_ATTACK, BUY_DEFENDER, 
		BUY_FARM, BUY_BANK, SETTINGS, MUTE_BACK,
		MUTE_EFFECTS, SAVE, EXIT_MAIN, EXIT_WINDOWS,
		SETTINGS_WINDOW, NULL_SEC,
	};	//Different areas requiring mouse input

#pragma region drawing
	/*
		Initialise the UI
	*/
	void initUI();

	/*
		Draw the UI
	*/
	void drawUI();

	/*
		Draws/deletes the settings from the window
	*/
	void drawSettings(int);

	/*
		Used to swap between send troops, indented send troops & attack button
	*/
	void changeButton(int);

	/*
		Updates an area of text on screen
	*/
	bool changeText(Text, int);
#pragma endregion

	/*
		Gets the section a point is in
	*/
	Section pointInBox(int, int);
};
#endif