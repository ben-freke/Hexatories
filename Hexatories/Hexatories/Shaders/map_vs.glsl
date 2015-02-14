#version 420
in vec2 position;
in vec3 colour;
in vec3 tex_coord;
out vec3 Colour;
out vec3 Tex_coord;
void main() {
	Colour = vec3(colour.x / 255, colour.y / 255, colour.z / 255);
	Tex_coord = tex_coord;
	gl_Position = vec4((position.x / 512) - 1, (position.y / 384) - 1, 0.0, 1.0);
}