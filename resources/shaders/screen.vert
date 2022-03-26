
#version 460 core


layout (location = 0) in vec2 a_position;
layout (location = 1) in vec2 a_text_coord;

out vec2 v_text_coord;


void main()
{
    v_text_coord = a_text_coord;
    gl_Position = vec4(a_position.x, a_position.y, 0.0, 1.0); 
}
