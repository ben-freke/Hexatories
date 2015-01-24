#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "Log.h"
#include "Shaders.h"
#include "HexMap.h"



//this is a test 5/3
// (C) Group 16

//This is the input test

int counter = 0;

int pnpoly(int nvert, float *vertx, float *verty, float testx, float testy)
/**

This function tests if an XY coordinate is in a given shape
nvert is the number of verticies in the given shape
vertx is an array of x verticies in the shape
verty is an array of y verticies in the shape
textx and testy are the XY locations in the shape to be tested (i.e. the mouse location)

**/
{
	int i, j, c = 0;
	for (i = 0, j = nvert - 1; i < nvert; j = i++) {
		if (((verty[i]>testy) != (verty[j]>testy)) &&
			(testx < (vertx[j] - vertx[i]) * (testy - verty[i]) / (verty[j] - verty[i]) + vertx[i]))
			c = !c;
	}
	return c;
}


void checkCursor(GLFWwindow* window){
	double xValue = NULL;
	double yValue = NULL;
	glfwGetCursorPos(window, &xValue, &yValue);
	/**
	xValue and yValue are variables to store the location of the mouse when the function is called.
	The GLFW function stores the XY positions in the locations of these variables
	**/

	float vertx[] = { 100, 200, 100, 200 };
	float verty[] = { 100, 100, 200, 200 };
	//Sets the verticies XY array for a square (i.e. from point 100,100 to point 200,200)



	if (pnpoly(4, vertx, verty, (float)xValue, (float)yValue)){
		std::string locationString = "X: " + std::to_string(xValue) + " Y: " + std::to_string(yValue) + " is correct\n";
		char const *pchar = locationString.c_str();
		//Runs the pnpoly function to test if the mouse is in the square. Will return true or false
		std::printf(pchar);
		counter++;
		//If in square, print.s
	}

}
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

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS){

		checkCursor(window);
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
		
		checkCursor(window);

		// Swap front and back buffers 
		glfwSwapBuffers(window);

		// Poll for and process events 
		glfwPollEvents();
		//checkCursor(window);
	}

	glfwTerminate();
	return 0;
}