#version 420
in vec3 Colour;
in vec2 Tex_coord;
out vec4 outColour;
uniform sampler2D tex;
void main() {
	if (Tex_coord.x == -1) {
		outColour = vec4(Colour, 1.0);
	} else {
		outColour = texture(tex, Tex_coord);
	}
}