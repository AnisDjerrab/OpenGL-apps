#version 430
out vec4 color;
in vec2 tc;
layout (binding=0) uniform sampler2D samp;
void main(void)
{ 
    color = texture(samp, tc);
}