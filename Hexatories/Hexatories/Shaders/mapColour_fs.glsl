#version 420
in vec3 Colour;
out vec4 outColour;
void main() {
	outColour = vec4(Colour, 1.0);
}