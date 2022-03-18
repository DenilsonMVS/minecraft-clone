
#version 460 core


uniform sampler2D u_texture;

in vec3 v_biome_color;
in vec2 v_main_text_coord;
in vec2 v_sec_text_coord;
in float v_bright;

out vec4 pixel_color;


void main() {
	const vec4 main_color = texture(u_texture, v_main_text_coord);
	const vec4 sec_color = texture(u_texture, v_sec_text_coord);

	const vec4 biome_color = vec4(v_biome_color.r * sec_color.r, v_biome_color.g * sec_color.g, v_biome_color.b * sec_color.b, sec_color.a);

	const vec4 join_color = biome_color + (1 - biome_color.a) * main_color;

	pixel_color = vec4(join_color.rgb * v_bright, 1);
}
