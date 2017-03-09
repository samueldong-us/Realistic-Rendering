#version 330 core
#define HdrMultiplier 1.0

layout (location = 0) out vec3 color;

uniform vec3 Color;

void main()
{
    color = HdrMultiplier * Color;
}