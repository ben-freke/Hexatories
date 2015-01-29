#version 420
in vec2 Tex_coord;
out vec4 outColour;
uniform sampler2D tex;
void main() {
	outColour = texture(tex, Tex_coord);
}
