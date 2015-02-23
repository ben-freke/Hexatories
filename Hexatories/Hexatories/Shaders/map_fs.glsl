#version 420
in vec3 Colour;
in vec3 Tex_coord;
out vec4 outColour;
uniform sampler2D border_tex;
uniform sampler2D map_tex;
uniform sampler2D farm_bank_tex;
void main() {
	if (Tex_coord.x == -1) {
		outColour = vec4(Colour, 1.0);
	} else {
		if (Tex_coord.z == 4) {
			outColour = texture(map_tex, vec2(Tex_coord.x, Tex_coord.y));
		} else if (Tex_coord.z < 4) {
			outColour = texture(border_tex, vec2((Tex_coord.x + Tex_coord.z) * 0.25, Tex_coord.y));
		} else {
			outColour = texture(farm_bank_tex, vec2((Tex_coord.x + Tex_coord.z - 5) * 0.5, Tex_coord.y)); 
		}
	}
}