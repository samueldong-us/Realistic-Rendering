#version 330 core

in vec3 uv;

layout (location = 0) out vec3 color;

uniform samplerCube Skymap;

void main()
{
    color = texture(Skymap, uv).rgb;
}