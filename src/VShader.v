#version 330 core

layout(location = 0) in vec3 pos;
uniform mat4 MVP;

layout(location = 1) in vec3 vert_color;
out vec3 frag_color;

void main() {
	vec4 v = vec4(pos, 1);
	gl_Position = MVP * v;

	frag_color = vert_color;
}
