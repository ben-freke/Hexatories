#version 420
in vec3 Colour;
in vec3 Tex_coord;
out vec4 outColour;
uniform sampler2D main_tex;
uniform sampler2D num_tex;
void main() {
	if (Tex_coord.z == 10) {
		outColour = texture(main_tex, vec2(Tex_coord.x, Tex_coord.y));
	} else {
		outColour = texture(num_tex, vec2((Tex_coord.x + Tex_coord.z) * 0.1, Tex_coord.y));
	}
}