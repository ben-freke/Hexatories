#version 420
in vec3 Tex_coord;
out vec4 outColour;
uniform sampler2D main_tex;
void main() {
	outColour = texture(main_tex, vec2((Tex_coord.x + Tex_coord.z) * 1/3, Tex_coord.y));
}