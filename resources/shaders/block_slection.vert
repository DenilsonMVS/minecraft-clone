
#version 460 core


layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texture_coord;

uniform vec3 u_offset;
uniform mat4 u_mvp;

out vec2 v_texture_coord;


void main() {
	gl_Position = u_mvp * vec4(u_offset + pos, 1);

	v_texture_coord = texture_coord;
}
