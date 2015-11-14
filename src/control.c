#include "control.h"

void calc_matrices(GLFWwindow* window, mat4x4* M) {
	static double last_time = -1;
	if (last_time == -1)
		last_time = glfwGetTime();
	double curr_time = glfwGetTime();
	float delta_time = (float) (curr_time - last_time);
	printf("FPS:\t%2.2f\n",pow(delta_time,-1));
	last_time = curr_time;

	static float hang = M_PI * 4.0f / 3.0f; // Horizontal angle (toward -Z)
	static float vang = M_PI / 3.0f; // Vertical angle (0 at the horizon)
	static float fov = M_PI / 4.0f; // 45 deg

	static float speed = 16.0f; // 4 units / second
	static float mouse_speed_factor = 250.0f;

	static vec3 pos = {3.5, 3, 3};
	double xpos, ypos;

	glfwGetCursorPos(window, &xpos, &ypos);
	glfwSetCursorPos(window, 1920/2, 1080/2); // Reset mouse

	// New orientation
	hang += (float) (1920/2 - xpos) / mouse_speed_factor;
	vang += (float) ( 1080/2 - ypos) / mouse_speed_factor;

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
		16.0f / 9.0f, // 4:3 ratio
		0.001f,        // display range : 0.1 unit <-> 100 units
		100000.0f
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
