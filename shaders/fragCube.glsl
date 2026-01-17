#version 430
out vec4 color;
in vec4 varyingColor;
void main(void)
{ 
    color = varyingColor;
}