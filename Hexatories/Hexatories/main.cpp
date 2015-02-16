#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SFML/Audio.hpp>
#include <SFML/Audio/Sound.hpp>
#include "Log.h"
#include "Shaders.h"
#include "Game.h"
#include "HexMap.h"
#include "Audio.h"
#include "Actions.h"

// (C) Group 16
using namespace std;

Audio gameMusic;
Audio swordClang;
Actions defaultActions;
bool firstClick = true;
Territory *firstTerr = NULL;

Game game;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
		double xValue = NULL;
		double yValue = NULL;

		if (firstClick){
			glfwGetCursorPos(window, &xValue, &yValue);
			if (game.getTerritory(xValue, yValue).getOwner() == 0){
				cout << "You must use a territory that is already owned... Stupid.\n";
			}
			else{
				firstTerr = &game.getTerritory(xValue, yValue);

				firstClick = false;
			}
			
		}

		else {
			glfwGetCursorPos(window, &xValue, &yValue);
			defaultActions.attack(game.getTerritory(xValue, yValue), *firstTerr, game);
			firstClick = true;
			firstTerr = NULL;
		}
		

	}

}

void curser_pos_callback(GLFWwindow *window, double x, double y) {
	
	game.highlightTerritory(x, y);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {


	if (key == GLFW_KEY_E && action == GLFW_PRESS) {


	}
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {


	}

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		defaultActions.increaseTurn();

	}

}

void _update_fps_counter(GLFWwindow* window) {

	double xValue = NULL;
	double yValue = NULL;
	glfwGetCursorPos(window, &xValue, &yValue);
	
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

	/*gameMusic.playAudio("sound.wav");
	gameMusic.fadeInAudio(0);
	
	
	srand(time(NULL));*/


	restart_log();

	// Initialize the library 
	if (!glfwInit())
		return -1;

	GLFWwindow* window = initWindow();

	if (!window) {
		glfwTerminate();
		return -1;
	}

	glDepthFunc(GL_NEVER);	//Renders in order drawn (last drawn on top)

	// Make the window's context current
	glfwMakeContextCurrent(window);

	glewExperimental = true;

	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		log("GLEW failed to initialise\n");
		return -1;
	}

	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, curser_pos_callback);
	glfwSetKeyCallback(window, key_callback);

	if (!game.initGame()) {
		log("Game init failed\n");
		return -1;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{

		_update_fps_counter(window);

		game.draw();
		
		// Swap front and back buffers 
		glfwSwapBuffers(window);

		// Poll for and process events 
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}