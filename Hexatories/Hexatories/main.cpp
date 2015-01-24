#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "Log.h"
#include "Shaders.h"
#include "HexMap.h"



//this is a test 5/3
// (C) Group 16

//This is the input test
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	double xValue = NULL;
	double yValue = NULL;
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){

		glfwGetCursorPos(window, &xValue, &yValue);

		std::string s = "X = " + (std::to_string((int)xValue)) + " Y = " + (std::to_string((int)yValue)) + "\n";
		char const *pchar = s.c_str();

		std::printf(pchar);
	}

}

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
	std::printf("Begin");
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

	glfwSetKeyCallback(window, key_callback);


	HexMap map;

	glViewport(142, 0, 740, 740);	// Basically changes the section of the window we draw to

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		_update_fps_counter(window);

		map.drawMap();
		
		// Swap front and back buffers 
		glfwSwapBuffers(window);

		// Poll for and process events 
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}