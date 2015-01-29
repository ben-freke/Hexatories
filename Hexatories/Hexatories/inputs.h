#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string>

#ifndef __INPUTS_H
	#define __INPUTS_H
	class inputs{

	public:
		int pnpoly(int nvert, float *vertx, float *verty, float testx, float testy);
		void initInputs(GLFWwindow* window);
		void checkCursor(GLFWwindow* window);
		void keyPress(GLFWwindow* window, int key, int action);
		void mouseClick(GLFWwindow* window, int button, int action);


	};
#endif
