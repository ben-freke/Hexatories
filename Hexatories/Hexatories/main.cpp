#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "Log.h"
#include "Shaders.h"
#include "HexMap.h"

//this is a test 5/3
// (C) Group 16

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	double xValue = NULL;
	double yValue = NULL;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		
		glfwGetCursorPos(window, &xValue, &yValue);

		log("MouseX: %f, MouseY: %f\n", xValue, yValue);
		
		int gridX, gridY;

		HexTile myHex;
		myHex.pointToTile(xValue, yValue, gridX, gridY);
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

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	HexMap map;

	double xValue = NULL;
	double yValue = NULL;


	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		char tmp[128];

		glfwGetCursorPos(window, &xValue, &yValue);
		sprintf_s(tmp, "Hexatories @ %.0f, %.0f", xValue, yValue);
		glfwSetWindowTitle(window, tmp);

		map.drawMap();
		
		// Swap front and back buffers 
		glfwSwapBuffers(window);

		// Poll for and process events 
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}