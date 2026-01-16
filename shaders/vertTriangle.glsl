#version 430
uniform float offset_x;
uniform float offset_y;
void main(void) {
    if (gl_VertexID == 0) gl_Position = vec4(-0.5, 0.0, 0.0, 1.0);
    else if (gl_VertexID == 1) gl_Position = vec4(-0.333333333333333, 0.333333333333333, 0.0, 1.0);
    else if (gl_VertexID == 2) gl_Position = vec4(0.333333333333333, 0.333333333333333, 0.0, 1.0);
    else if (gl_VertexID == 3) gl_Position = vec4(0.5, 0.0, 0.0, 1.0);
    else if (gl_VertexID == 4) gl_Position = vec4(0.333333333333333, -0.333333333333333, 0.0, 1.0);
    else if (gl_VertexID == 5) gl_Position = vec4(-0.333333333333333, -0.333333333333333, 0.0, 1.0);
    gl_Position.x += offset_x;
    gl_Position.y += offset_y;
} 