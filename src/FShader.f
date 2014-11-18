#version 330 core

out vec3 color;

void main() {
	color = vec3(
		gl_FragCoord.x / 1024.0,
		gl_FragCoord.y / 768.0,
		0.5
	);
}
