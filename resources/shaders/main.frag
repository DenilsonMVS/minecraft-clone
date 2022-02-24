
#version 460 core


uniform sampler2D u_texture;

in vec2 text_coord;

out vec4 pixel_color;


void main() {
	pixel_color = texture(u_texture, text_coord);
}
