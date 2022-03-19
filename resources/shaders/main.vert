
#version 460 core


layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 biome_color;
layout (location = 2) in vec2 main_text_coord;
layout (location = 3) in vec2 sec_text_coord;
layout (location = 4) in float bright;

out vec3 v_biome_color;
out vec2 v_main_text_coord;
out vec2 v_sec_text_coord;
out float v_bright;

uniform mat4 u_mvp;
uniform vec3 u_offset;


void main() {
	gl_Position = u_mvp * vec4(pos + u_offset, 1);

	v_biome_color = biome_color;
	v_main_text_coord = main_text_coord;
	v_sec_text_coord = sec_text_coord;
	v_bright = bright;
}
