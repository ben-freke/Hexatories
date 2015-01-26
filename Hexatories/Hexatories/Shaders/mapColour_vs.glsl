#version 420
in vec2 position;
in vec3 colour;
out vec3 Colour;
void main() {
	Colour = colour;
	gl_Position = vec4((position.x / 512) - 1, (position.y / 384) - 1, 0.0, 1.0);
}
