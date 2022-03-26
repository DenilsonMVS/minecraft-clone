
#version 460 core


uniform sampler2D u_color;
uniform sampler2D u_depth;

in vec2 v_text_coord;

out vec4 FragColor;


void main()
{
    const float other_depth = texture(u_depth, v_text_coord).r;
    const float current_depth = gl_FragCoord.z;

    if(current_depth >= other_depth)
        discard;
    
    gl_FragDepth = other_depth;
    FragColor = texture(u_color, v_text_coord);
}
