
#version 460 core


layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 text_coord;
layout (location = 2) in float bright;

out vec2 v_text_coord;
out float v_bright;

uniform mat4 u_mvp;
uniform vec3 u_offset;


void main() {
	gl_Position = u_mvp * vec4(pos + u_offset, 1);

	v_text_coord = text_coord;
	v_bright = bright;
}
