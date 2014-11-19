#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include <glew.h>
#include <GLFW/glfw3.h>

#include "linmath.h"
#include "Shaders.h"

// Global vars
GLFWwindow* window;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

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

	window = glfwCreateWindow(1024, 768, "Hello, World", NULL, NULL);
	if (!window) {
		fprintf(stderr, "glfw: failed to create window\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	glewExperimental = 1; // Need this for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "glew init failed\n");
		glfwTerminate();
		return -1;
	}

	// Pre loop
	////////
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST); // Enable depth test
	glDepthFunc(GL_LESS); // Accept frag if closer to camera than former

	GLuint vert_arr;
	glGenVertexArrays(1, &vert_arr);
	glBindVertexArray(vert_arr);

	// Make GLSL program from shaders
	GLuint prog = load_shaders("src/VShader.v", "src/FShader.f");

	// Cube has 6 faces, 2 tri each, so 6*2=12 tri, and 12*3 vertices
	GLfloat vertices[] = {
		-1.0f, -1.0f, -1.0f, // triangle 1 : begin
		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f, // triangle 1 : end
		 1.0f,  1.0f, -1.0f, // triangle 2 : begin
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f, // triangle 2 : end
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	GLuint vert_buff;
	glGenBuffers(1, &vert_buff);
	glBindBuffer(GL_ARRAY_BUFFER, vert_buff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Color : vertex (randomly generated)
	GLfloat colors[] = {
		0.583f,  0.771f,  0.014f,
		0.609f,  0.115f,  0.436f,
		0.327f,  0.483f,  0.844f,

		0.822f,  0.569f,  0.201f,
		0.435f,  0.602f,  0.223f,
		0.310f,  0.747f,  0.185f,

		0.435f,  0.602f,  0.223f,
		0.822f,  0.569f,  0.201f,
		0.359f,  0.583f,  0.152f,

		0.483f,  0.596f,  0.789f,
		0.195f,  0.548f,  0.859f,
		0.559f,  0.861f,  0.639f,

		0.483f,  0.596f,  0.789f,
		0.559f,  0.861f,  0.639f,
		0.771f,  0.328f,  0.970f,

		0.676f,  0.977f,  0.133f,
		0.583f,  0.771f,  0.014f,
		0.327f,  0.483f,  0.844f,

		// ^ 6 v

		0.997f,  0.513f,  0.064f,
		0.945f,  0.719f,  0.592f,
		0.543f,  0.021f,  0.978f,

		0.279f,  0.317f,  0.505f,
		0.167f,  0.620f,  0.077f,
		0.347f,  0.857f,  0.137f,

		0.167f,  0.620f,  0.077f,
		0.279f,  0.317f,  0.505f,
		0.783f,  0.290f,  0.734f,

		0.722f,  0.645f,  0.174f,
		0.225f,  0.587f,  0.040f,
		0.302f,  0.455f,  0.848f,

		0.722f,  0.645f,  0.174f,
		0.302f,  0.455f,  0.848f,
		0.053f,  0.959f,  0.120f,

		0.673f,  0.211f,  0.457f,
		0.997f,  0.513f,  0.064f,
		0.543f,  0.021f,  0.978f
	};
	// GLfloat colors[] = {
	// 	0.0f,  0.0f,  0.0f,
	// 	0.0f,  0.0f,  0.0f,
	// 	0.0f,  0.0f,  0.0f,

	// 	0.0f,  0.0f,  0.0f,
	// 	0.0f,  0.0f,  0.0f,
	// 	0.0f,  0.0f,  0.0f,

	// 	0.0f,  0.0f,  0.0f,
	// 	0.0f,  0.0f,  0.0f,
	// 	0.0f,  0.0f,  0.0f,

	// 	0.0f,  0.0f,  0.0f,
	// 	0.0f,  0.0f,  0.0f,
	// 	0.0f,  0.0f,  0.0f,

	// 	0.0f,  0.0f,  0.0f,
	// 	0.0f,  0.0f,  0.0f,
	// 	0.0f,  0.0f,  0.0f,

	// 	0.0f,  0.0f,  0.0f,
	// 	0.0f,  0.0f,  0.0f,
	// 	0.0f,  0.0f,  0.0f,

	// 	// ^ 6 v

	// 	0.0f,  1.0f,  1.0f, // 1.1
	// 	0.0f,  0.5f,  1.0f, // 1.3
	// 	1.0f,  0.0f,  1.0f, // 1.2

	// 	0.0f,  1.0f,  0.0f, // 2.1
	// 	0.0f,  0.0f,  1.0f, // 2.2
	// 	1.0f,  0.0f,  0.0f, // 2.3

	// 	0.0f,  0.0f,  1.0f, // 2.2
	// 	0.0f,  1.0f,  0.0f, // 2.1
	// 	1.0f,  0.0f,  0.0f, // 2.4

	// 	1.0f,  0.0f,  0.0f, // 3.1
	// 	0.0f,  0.0f,  1.0f, // 3.3
	// 	0.0f,  1.0f,  0.0f, // 3.2

	// 	1.0f,  0.0f,  0.0f, // 3.1
	// 	0.0f,  1.0f,  0.0f, // 3.2
	// 	0.0f,  0.0f,  1.0f, // 3.4

	// 	0.5f,  0.0f,  1.0f, // 1.4
	// 	0.0f,  1.0f,  1.0f, // 1.1
	// 	1.0f,  0.0f,  1.0f  // 1.2
	// };
	GLuint color_buff;
	glGenBuffers(1, &color_buff);
	glBindBuffer(GL_ARRAY_BUFFER, color_buff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

	mat4x4 projection, view, model;
	mat4x4_perspective(
		projection,
		M_PI / 4.0f, // 45 deg fov
		4.0f / 3.0f, // 4:3 ratio
		0.1f,        // display range : 0.1 unit <-> 100 units
		100.0f
	);
	mat4x4_look_at(
		view,
		(vec3) {4.0f, 3.0f, 4.0f}, // eye
		(vec3) {0.0f, 0.0f, 0.0f}, // center
		(vec3) {0.0f, 1.0f, 0.0f}  // up
	);
	mat4x4_identity(model);

	// projection * view * model
	mat4x4 temp, MVP;
	mat4x4_mul(temp, projection, view);
	mat4x4_mul(MVP, temp, model);

	GLuint matrix = glGetUniformLocation(prog, "MVP");
	////////

	while (!glfwWindowShouldClose(window)) {
		// Render
		////////
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(prog); // Use shaders
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
