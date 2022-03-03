
#version 460 core


layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 text_pos;

out vec2 text_coord;

uniform mat4 u_mvp;


void main() {
	gl_Position = u_mvp * vec4(pos, 0, 1);

	text_coord = text_pos;
}
