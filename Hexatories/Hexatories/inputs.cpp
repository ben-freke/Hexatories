#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <stdio.h>
#include <fstream>
#include "inputs.h"


void inputs::keyPress(GLFWwindow* window, int key, int action){
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
void inputs::mouseClick(GLFWwindow* window, int button, int action){
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		checkCursor(window);
}
int inputs::pnpoly(int nvert, float *vertx, float *verty, float testx, float testy)
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

void inputs::checkCursor(GLFWwindow* window){
	double xValue = NULL;
	double yValue = NULL;
	glfwGetCursorPos(window, &xValue, &yValue);
	/**
	xValue and yValue are variables to store the location of the mouse when the function is called.
	The GLFW function stores the XY positions in the locations of these variables
	**/

	float vertx[] = { 100, 200, 0, 300, 100, 200 };
	float verty[] = { 0, 0, 100, 100, 200, 200 };
	//Sets the verticies XY array for a square (i.e. from point 100,100 to point 200,200)



	if (pnpoly(4, vertx, verty, (float)xValue, (float)yValue)){
		std::string locationString = "X: " + std::to_string(xValue) + " Y: " + std::to_string(yValue) + " is correct\n";
		char const *pchar = locationString.c_str();
		//Runs the pnpoly function to test if the mouse is in the square. Will return true or false
		std::printf(pchar);
		//If in square, print.s
	}

}


