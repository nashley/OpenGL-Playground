#version 330 core

layout(location = 0) in vec3 pos;
uniform mat4 MVP;
uniform bool line;

out vec3 frag_color;

void main() {
	vec4 v = vec4(pos, 1);
	gl_Position = MVP * v;

	if (line) {
		frag_color = vec3(0.043, 0.709, 1);
	}
	else {
		frag_color = vec3(0.067, 0.067, 0.067);
	}
}
