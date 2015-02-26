#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Log.h"
#include "Game.h"

// (C) Group 16
using namespace std;

Game game;
bool endGame = false;

/*
	Passes mouse click input to Game to be processed
*/
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){

		double xValue = NULL;
		double yValue = NULL;

		glfwGetCursorPos(window, &xValue, &yValue);
		if (game.handleMouseInput(xValue, yValue, true, false)) {	// true == clicked (not hover) false = don't reset selected ters
			endGame = true;
		}
	}

}

/*
	Passes mouse over input to Game to be processed
*/
void curser_pos_callback(GLFWwindow *window, double x, double y) {

	game.handleMouseInput(x, y, false, false);	// false == mouseover not click, false = don't reset selected ters
}

/*
	Passes key input to Game to be processed
*/
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (action == GLFW_PRESS)
		game.handleKeyInput(key);

}

/*
	Adds FPS to window title
*/
void _update_fps_counter(GLFWwindow* window) {

	static double previous_seconds = glfwGetTime();
	static int frame_count;
	double current_seconds = glfwGetTime();
	double elapsed_seconds = current_seconds - previous_seconds;

	if (elapsed_seconds > 0.25) {

		previous_seconds = current_seconds;
		double fps = (double)frame_count / elapsed_seconds;
		char tmp[128];

		sprintf_s(tmp, "Hexatories @ fps: %.2f", fps);
		glfwSetWindowTitle(window, tmp);
		frame_count = 0;
	}

	frame_count++;

}

/*
	Sets up the game window
*/
GLFWwindow* initWindow(void) {

	//glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow* window = NULL;

	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());	// Get screen resolution
	int window_width = mode->width;
	int window_height = mode->height;

	if (window_width >= 1024 || window_height >= 768) {		// Ensures resolution is big enough and if necessary makes window fullscreen

		if (window_width == 1024 || window_height == 768) {
			window = glfwCreateWindow(1024, 768, "Hexatories", glfwGetPrimaryMonitor(), NULL);
		} else {
			window = glfwCreateWindow(1024, 768, "Hexatories", NULL, NULL);
		}
	}

	return window;
}

int main(void) {
	
	

	srand((unsigned int)time(NULL));

	restart_log();	// Clear the debug log

	// Initialize glfw
	if (!glfwInit())
		return -1;

	GLFWwindow* window = initWindow();

	if (!window) {
		glfwTerminate();
		return -1;
	}

	glDepthFunc(GL_NEVER);	//Renders in order drawn (last drawn on top)

	glfwMakeContextCurrent(window);

	glewExperimental = true;

	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		log("GLEW failed to initialise\n");
		return -1;
	}

	// set input callbacks (basically event handlers)
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, curser_pos_callback);
	glfwSetKeyCallback(window, key_callback);

	game.initGame();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		if (endGame) break;
		_update_fps_counter(window);

		game.draw();
		
		glfwSwapBuffers(window);

		// Poll for events (i.e. input)
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}