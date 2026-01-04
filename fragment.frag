#version 330 core

in vec3 vertexColor;
out vec4 FragColor;

uniform sampler2D texture1;

void main(void)
{
    FragColor = vec4(vertexColor, 1.0);
}
