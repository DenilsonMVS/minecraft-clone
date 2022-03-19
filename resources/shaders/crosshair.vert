
#version 460 core


uniform int u_width;
uniform int u_height;

layout (location = 0) in vec2 pos;

void main() {
	const float width = u_width;
	const float height = u_height;

	gl_Position = vec4(pos.x / width, pos.y / height, 0, 1);
}
