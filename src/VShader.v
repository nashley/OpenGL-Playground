#version 330 core

in vec3 pos;
uniform mat4 MVP;

void main() {
	vec4 v = vec4(pos, 1);
	gl_Position = MVP * v;
}
