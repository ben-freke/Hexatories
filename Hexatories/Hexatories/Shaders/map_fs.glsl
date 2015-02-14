#version 420
in vec3 Colour;
in vec3 Tex_coord;
out vec4 outColour;
uniform sampler2D white_tex;
uniform sampler2D blue_tex;
uniform sampler2D red_tex;
uniform sampler2D map_tex;
void main() {
	if (Tex_coord.x == -1) {
		outColour = vec4(Colour, 1.0);
	} else {
		if (Tex_coord.z == 0) {
			outColour = texture(white_tex, vec2(Tex_coord.x, Tex_coord.y));
		} 
		if (Tex_coord.z == 1) {
			outColour = texture(blue_tex, vec2(Tex_coord.x, Tex_coord.y));
		} 
		if (Tex_coord.z == 2) {
			outColour = texture(red_tex, vec2(Tex_coord.x, Tex_coord.y));
		} 
		if (Tex_coord.z == 3) {
			outColour = texture(map_tex, vec2(Tex_coord.x, Tex_coord.y));
		}
	}
}