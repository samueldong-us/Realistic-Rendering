#version 330 core

in vec3 position;
in vec2 uv;

uniform vec3 LightPosition;
uniform float FarPlane;

uniform sampler2D texture_mask1;

void main()
{
    if (texture(texture_mask1, uv).r < 0.5)
    {
        discard;
    }
    gl_FragDepth = length(LightPosition - position) / FarPlane;
}