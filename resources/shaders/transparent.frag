
#version 460 core


uniform sampler2D u_texture;

in vec2 v_text_coord;
in float v_bright;


out vec4 pixel_color;

void main() {
	const vec4 color = texture(u_texture, v_text_coord);

	pixel_color = vec4(color.rgb * v_bright, color.a);
}
