#version 330 core

layout(location = 0) in vec3 pos;
uniform mat4 MVP;
uniform bool line;

layout(location = 1) in vec3 vert_color;
out vec3 frag_color;

void main() {
	vec4 v = vec4(pos, 1);
	gl_Position = MVP * v;

	if (line) {
		frag_color = vec3(0, 0, 0.16);
	}
	else {
		frag_color[0] = 0.15;
		frag_color[1] = 0.15;
		frag_color[2] = 1 - (abs(pos.y + 1) / 2.0);
	}
}
