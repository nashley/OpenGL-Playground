#version 330 core

layout(location = 0) in vec3 pos;
uniform mat4 MVP;

out vec3 frag_color;

void main() {
	vec4 v = vec4(pos, 1);
	gl_Position = MVP * v;

	frag_color[0] = (abs(pos.z + 4) / 8.0);
	frag_color[1] = (abs(pos.y + 1) / 2.0);
	frag_color[2] = (abs(pos.x + 4) / 8.0);
}
