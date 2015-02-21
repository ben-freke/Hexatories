#include <vector>
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "Territory.h"

#ifndef __GAMEUI_H
#define __GAMEUI_H

class gameUI {

	static const int sectionCoords[];

	GLint uniforms[3];
	GLuint vao, vbo, ebo, prog;
	std::vector<GLint> verts;
	std::vector<GLushort> indices;

	void updateVAO();
	void setupVAO();

	void mainOverlay();
	void initText();

public:

	enum class Text {
		COINS = 0, TOTAL_POP, TOTAL_ATK, TOTAL_DEF,
		POP, ATK, DEF, SEND_ATK, SEND_DEF,
	};

	enum class Section {
		MAP = 0, SEND_TROOPS, ATK_UP, ATK_DOWN,
		DEF_UP, DEF_DOWN, BUY_ATTACK, BUY_DEFENDER, 
		BUY_FARM, BUY_BANK, SETTINGS, CLOSE, NULL_SEC,
	};

	void initUI();
	void drawUI();

	void attackButton(bool);

	bool changeText(Text, int);
	Section pointInBox(int, int);
};
#endif