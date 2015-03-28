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

	GLfloat tmp_vertices[] = {
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

	typedef struct {
		// Cube has 6 faces, 2 tri each, so 6*2=12 tri, and 12*3 vertices
		GLfloat vertices[108];
		GLuint vert_buff;
	} cube;

	cube c1;
	cube c2;

	cube cubes[16];

	for (int i = 0; i < 108; i += 3) {
		int j = 0;
		for (int x = -6; x < 6; x += 3) {
			for (int y = -6; y < 6; y += 3) {
				cubes[j].vertices[i  ] = tmp_vertices[i  ] + x; // x
				cubes[j].vertices[i+1] = tmp_vertices[i+1]    ; // y
				cubes[j].vertices[i+2] = tmp_vertices[i+2] + y; // z
				j++;
			}
		}
	}

	for (int j = 0; j < 16; j++) {
		glGenBuffers(1, &cubes[j].vert_buff);
		glBindBuffer(GL_ARRAY_BUFFER, cubes[j].vert_buff);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubes[j].vertices), cubes[j].vertices, GL_STATIC_DRAW);
	}

	GLuint matrix = glGetUniformLocation(prog, "MVP");
	mat4x4 MVP;
	////////

	while (
		!glfwWindowShouldClose(window) &&
		glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
	) {
		// Render
		////////
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(prog); // Use shaders

		calc_matrices(window, &MVP);
		glUniformMatrix4fv(matrix, 1, GL_FALSE, &MVP[0][0]);

		for (int j = 0; j < 16; j++) {
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, cubes[j].vert_buff);
			glVertexAttribPointer(
				0,                  // attribute
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*) 0           // array buffer offset
			);
			glDrawArrays(GL_TRIANGLES, 0, 12*3);
		}

		glDisableVertexAttribArray(0);
		////////

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup
	glDeleteVertexArrays(1, &vert_arr);
	glDeleteVertexArrays(1, &c1.vert_buff);
	glDeleteVertexArrays(1, &c2.vert_buff);
	glDeleteProgram(prog);

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}
