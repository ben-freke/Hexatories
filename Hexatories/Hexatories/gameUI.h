#include <vector>
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "Territory.h"

#ifndef __GAMEUI_H
#define __GAMEUI_H

class gameUI {
#pragma region vars
	static const int sectionCoords[];

	GLint uniforms[3];
	GLuint vao, vbo, ebo, prog;
	std::vector<GLint> verts;
	std::vector<GLushort> indices;

	bool firstTime = true;
#pragma endregion

#pragma region openglStuff
	void setupVAO();
	void updateVAO();
	void mainOverlay();
#pragma endregion

	void initText();

public:

	enum class Text {
		COINS = 0, TOTAL_POP, TOTAL_ATK, TOTAL_DEF,
		POP, ATK, DEF, SEND_ATK, SEND_DEF, SCORE, ROUND,
	};

	enum class Section {
		MAP = 0, SEND_TROOPS, ATK_UP, ATK_DOWN,
		DEF_UP, DEF_DOWN, BUY_ATTACK, BUY_DEFENDER, 
		BUY_FARM, BUY_BANK, SETTINGS, CLOSE, NULL_SEC,
	};

	void initUI();
	void drawUI();

	void changeButton(int);

	bool changeText(Text, int);
	Section pointInBox(int, int);
};
#endif