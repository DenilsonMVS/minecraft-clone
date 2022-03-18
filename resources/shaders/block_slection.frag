
#version 460 core


uniform sampler2D u_texture;

in vec2 v_texture_coord;

out vec4 pixel_color;


void main() {
	pixel_color = texture(u_texture, v_texture_coord);
}
