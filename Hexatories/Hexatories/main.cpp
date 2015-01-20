#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "Textures.h"

//this is a test 5/3
// (C) Group 16

class Hex {

	GLuint vaoid, vboid;
	
	public:
	int initHex(int x, int y) {
		glGenVertexArrays(1, &vaoid);
		glBindVertexArray(vaoid);
		// Tells OpenGL that we are going to be using the vaoid array

		GLfloat verts[] = {
			-0.98125f, 0.964375f,		// left
			-0.96875f, 0.940625f,		// bottom left
			-0.94375f, 0.940625f,		// bottom right
			-0.93125f, 0.964375f,		// right
			-0.94375f, 0.988125f,		// top right
			-0.96875f, 0.988125f,		// top left
		};

		int xoff = x % 2;

		for (int i = 0; i < 14; i += 2) {
			verts[i] += 0.0375f * x;
			verts[i + 1] -= 0.0475f * y + 0.02375f * xoff;
		}

		glGenBuffers(1, &vboid);
		glBindBuffer(GL_ARRAY_BUFFER, vboid);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

		return 0;
	}

	int drawHex() {

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vboid);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		short indices[] = { 0, 1, 2, 3, 4, 5};

		glDrawElements(GL_POLYGON, 6, GL_UNSIGNED_SHORT, indices);

		glDisableVertexAttribArray(0);

		return 0;
	}
};

void _update_fps_counter(GLFWwindow* window) {

	static double previous_seconds = glfwGetTime();
	static int frame_count;
	double current_seconds = glfwGetTime();
	double elapsed_seconds = current_seconds - previous_seconds;

	if (elapsed_seconds > 0.25) {

		previous_seconds = current_seconds;
		double fps = (double)frame_count / elapsed_seconds;
		char tmp[128];

		GLint m_viewport[4];
		glGetIntegerv(GL_VIEWPORT, m_viewport);

		sprintf_s(tmp, "Hexatories @ fps: %.2f", fps);
		glfwSetWindowTitle(window, tmp);
		frame_count = 0;
	}

	frame_count++;

}

GLFWwindow* initWindow(void) {

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

	// Initialize the library 
	if (!glfwInit())
		return -1;

	GLFWwindow* window = initWindow();

	if (!window) {
		glfwTerminate();
		return -1;
	}

	// Make the window's context current
	glfwMakeContextCurrent(window);

	glewExperimental = true;

	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		fprintf(stderr, "GLEW Failed to initalise");
		return -1;
	}

	glViewport(142, 0, 740, 740);	// Basically changes the section of the window we draw to

	Hex myHex[52][41];
	for (int x = 0; x < 52; x++) {
		for (int y = 0; y < 41; y++) {
			myHex[x][y].initHex(x, y);
		}
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		_update_fps_counter(window);

		// Render here 
		for (int x = 0; x < 52; x++) {
			for (int y = 0; y < 41; y++) {
				myHex[x][y].drawHex();
			}
		}

		// Swap front and back buffers 
		glfwSwapBuffers(window);

		// Poll for and process events 
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}