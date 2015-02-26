#version 420
in vec3 Colour;
in vec3 Tex_coord;
out vec4 outColour;
uniform sampler2D main_tex;
uniform sampler2D num_tex;
uniform sampler2D button_tex;
uniform sampler2D set_tex;
uniform sampler2D mute_tex;
uniform sampler2D victory_tex;
void main() {
	if (Tex_coord.z == 10) {
		outColour = texture(main_tex, vec2(Tex_coord.x, Tex_coord.y));
	} else  if (Tex_coord.z == 11 || Tex_coord.z == 12) {
		outColour = texture(button_tex, vec2((Tex_coord.x + Tex_coord.z - 11) * 0.5, Tex_coord.y)); 
	} else if (Tex_coord.z == 13) {
		outColour = texture(set_tex, vec2(Tex_coord.x, Tex_coord.y));
	} else if (Tex_coord.z == 14) {
		outColour = texture(mute_tex, vec2(Tex_coord.x, Tex_coord.y));
	} else if (Tex_coord.z == 15 || Tex_coord.z == 16) {
		outColour = texture(victory_tex, vec2((Tex_coord.x + Tex_coord.z - 15) * 0.5, Tex_coord.y)); 
	} else {
		outColour = texture(num_tex, vec2((Tex_coord.x + Tex_coord.z) * 0.1, Tex_coord.y));
	}
}