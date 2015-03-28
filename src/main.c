#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>

#include <glew.h>
#include <GLFW/glfw3.h>

#include "util/linmath.h"
#include "util/Shaders.h"
#include "control.h"

int main() {
	if (!glfwInit()) {
		fprintf(stderr, "glfw init failed\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window;
	window = glfwCreateWindow(1024, 768, "Hello, World", NULL, NULL);
	if (!window) {
		fprintf(stderr, "glfw: failed to create window\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = 1; // Need this for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "glew init failed\n");
		glfwTerminate();
		return -1;
	}

	// Pre loop
	////////
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPos(window, 1024/2, 768/2);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST); // Enable depth test
	glDepthFunc(GL_LESS); // Accept frag if closer to camera than former
	glEnable(GL_CULL_FACE);
		// Cull triangles that arent towards camera
		// Faster because it only renders objects in view

	GLuint vert_arr;
	glGenVertexArrays(1, &vert_arr);
	glBindVertexArray(vert_arr);

	// Make GLSL program from shaders
	GLuint prog = load_shaders("src/shaders/VShader.v", "src/shaders/FShader.f");

	// Cube has 6 faces, 2 tri each, so 6*2=12 tri, and 12*3 vertices
	GLfloat vertices[] = {
		-1.0f, -1.0f, -1.0f,//
		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,//
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,//
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,//
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,//
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,//
		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,

		-1.0f,  1.0f,  1.0f,//
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,//
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,//
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,//
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,//
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,//
		-1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	GLuint vert_buff;
	glGenBuffers(1, &vert_buff);
	glBindBuffer(GL_ARRAY_BUFFER, vert_buff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Color : vertex (randomly generated)
	srand(time(NULL));
	GLfloat colors[108];
	for (int i = 0; i < 108; i++)
		colors[i] = rand() % 2;

	GLuint color_buff;
	glGenBuffers(1, &color_buff);
	glBindBuffer(GL_ARRAY_BUFFER, color_buff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

	GLuint matrix = glGetUniformLocation(prog, "MVP");
	mat4x4 MVP;
	GLuint line = glGetUniformLocation(prog, "line");
	////////

	while (
		!glfwWindowShouldClose(window) &&
		glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
	) {
		// Render
		////////
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Draw outlines
		glUniform1d(line, 1);
		glUseProgram(prog); // Use shaders

		calc_matrices(window, &MVP);
		glUniformMatrix4fv(matrix, 1, GL_FALSE, &MVP[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vert_buff);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*) 0           // array buffer offset
		);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, color_buff);
		glVertexAttribPointer(
			1,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*) 0           // array buffer offset
		);

		glDrawArrays(GL_TRIANGLES, 0, 12*3);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Draw colors
		glUniform1d(line, 0);
		glUseProgram(prog); // Shaders again
		glDrawArrays(GL_TRIANGLES, 0, 12*3); // Redraw

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		////////

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup
	glDeleteVertexArrays(1, &vert_arr);
	glDeleteVertexArrays(1, &vert_buff);
	glDeleteProgram(prog);

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}
