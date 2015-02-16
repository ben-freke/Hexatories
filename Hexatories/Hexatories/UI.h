#include <vector>
#include <GL\glew.h>
#include <GLFW/glfw3.h>

class UI {

	enum class Text { 
		COINS = 0, POP, TOTAL_POP, ATK, TOTAL_ATK,
		DEF, TOTAL_DEF, SEND_ATK, SEND_DEF,
	};

	enum class UpdateType {
		RELATIVE, SET,
	};

	GLint uniforms[2];
	GLuint vao, vbo, prog;
	std::vector<GLint> verts;

	void updateVAO();
	void setupVAO(std::vector<GLushort>);

	void mainOverlay();
	void initText();

public:

	void initUI();
	void drawUI();
	void changeText(Text, UpdateType, int);
};