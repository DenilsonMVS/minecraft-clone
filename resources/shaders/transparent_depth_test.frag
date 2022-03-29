
#version 460 core

uniform sampler2D u_texture;
uniform sampler2D u_depth;
uniform ivec2 u_window_dimensions;

in vec2 v_text_coord;
in float v_bright;


out vec4 pixel_color;

void main() {
	if(gl_FragCoord.z <= texture(u_depth, gl_FragCoord.xy / u_window_dimensions).r)
		discard;

	const vec4 color = texture(u_texture, v_text_coord);
	pixel_color = vec4(color.rgb * v_bright, color.a);
}
