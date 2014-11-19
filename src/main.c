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
vec3 pos = {3.5, 3, 3};

void calc_matrices(mat4x4* M) {
	static double last_time = -1;
	if (last_time == -1)
		last_time = glfwGetTime();
	double curr_time = glfwGetTime();
	float delta_time = (float) (curr_time - last_time);
	last_time = curr_time;

	static float hang = M_PI * 4.0f / 3.0f; // Horizontal angle (toward -Z)
	static float vang = M_PI / 3.0f; // Vertical angle (0 at the horizon)
	static float fov = M_PI / 4.0f; // 45 deg

	static float speed = 4.0f; // 4 units / second
	static float mouse_speed = 0.004f;

	double xpos, ypos;

	glfwGetCursorPos(window, &xpos, &ypos);
	glfwSetCursorPos(window, 1024/2, 768/2); // Reset mouse

	// New orientation
	hang += mouse_speed * (float) (1024/2 - xpos);
	vang += mouse_speed * (float) ( 768/2 - ypos);

	vec3 dir = {
		cosf(vang) * sinf(hang),
		sinf(vang),
		cosf(vang) * cosf(hang)
	},
	right = {
		sinf(hang - M_PI / 2.0f),
		0,
		cosf(hang - M_PI / 2.0f)
	};

	vec3 up;
	vec3_mul_cross(up, right, dir);

	vec3 temp;
	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		vec3_scale(temp, dir, delta_time * speed);
		vec3_add(pos, pos, temp);
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		vec3_scale(temp, dir, delta_time * speed);
		vec3_sub(pos, pos, temp);
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		vec3_scale(temp, right, delta_time * speed);
		vec3_add(pos, pos, temp);
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		vec3_scale(temp, right, delta_time * speed);
		vec3_sub(pos, pos, temp);
	}
	// Move up
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		vec3_scale(temp, up, delta_time * speed);
		vec3_add(pos, pos, temp);
	}
	// Move down
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		vec3_scale(temp, up, delta_time * speed);
		vec3_sub(pos, pos, temp);
	}

	mat4x4 projection, view, model;
	mat4x4_perspective(
		projection,
		fov,         // in radians
		4.0f / 3.0f, // 4:3 ratio
		0.1f,        // display range : 0.1 unit <-> 100 units
		100.0f
	);
	vec3 center;
	vec3_add(center, pos, dir);
	mat4x4_look_at(
		view,
		pos,    // eye
		center, // center
		up      // up
	);
	mat4x4_identity(model);

	// projection * view * model
	mat4x4 tempM;
	mat4x4_mul(tempM, projection, view);
	mat4x4_mul(*M, tempM, model);
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
	// glEnable(GL_CULL_FACE);
		// Cull triangles that arent towards camera
		// Disables noclip but is faster

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
		0.483f,  0.596f,  0.789f,//
		0.195f,  0.548f,  0.859f,
		0.559f,  0.861f,  0.639f,
		0.435f,  0.602f,  0.223f,//
		0.822f,  0.569f,  0.201f,
		0.359f,  0.583f,  0.152f,
		0.327f,  0.483f,  0.844f,//
		0.583f,  0.771f,  0.014f,
		0.609f,  0.115f,  0.436f,
		0.435f,  0.602f,  0.223f,//
		0.310f,  0.747f,  0.185f,
		0.822f,  0.569f,  0.201f,
		0.483f,  0.596f,  0.789f,//
		0.559f,  0.861f,  0.639f,
		0.771f,  0.328f,  0.970f,
		0.327f,  0.483f,  0.844f,//
		0.676f,  0.977f,  0.133f,
		0.583f,  0.771f,  0.014f,

		0.997f,  0.513f,  0.064f,//
		0.945f,  0.719f,  0.592f,
		0.543f,  0.021f,  0.978f,
		0.279f,  0.317f,  0.505f,//
		0.167f,  0.620f,  0.077f,
		0.347f,  0.857f,  0.137f,
		0.167f,  0.620f,  0.077f,//
		0.279f,  0.317f,  0.505f,
		0.783f,  0.290f,  0.734f,
		0.722f,  0.645f,  0.174f,//
		0.225f,  0.587f,  0.040f,
		0.302f,  0.455f,  0.848f,
		0.722f,  0.645f,  0.174f,//
		0.302f,  0.455f,  0.848f,
		0.053f,  0.959f,  0.120f,
		0.673f,  0.211f,  0.457f,//
		0.997f,  0.513f,  0.064f,
		0.543f,  0.021f,  0.978f
	};
	GLuint color_buff;
	glGenBuffers(1, &color_buff);
	glBindBuffer(GL_ARRAY_BUFFER, color_buff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

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

		calc_matrices(&MVP);
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
