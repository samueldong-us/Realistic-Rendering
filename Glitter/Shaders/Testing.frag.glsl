#version 330 core
#define Gamma 2.2
#define Exposure 0.1

in vec2 uv;

out vec3 color;

uniform sampler2D Buffer;

void main()
{
    vec3 hdrColor = texture(Buffer, uv).rgb;
    vec3 ldrColor = vec3(1.0) - exp(-hdrColor * Exposure);
    color = pow(ldrColor, vec3(1.0 / 2.2));
}