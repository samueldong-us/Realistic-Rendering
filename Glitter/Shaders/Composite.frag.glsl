#version 330 core
#define AmbientTerm 0.01

in vec2 uv;

layout (location = 0) out vec3 compositeColor;

uniform sampler2D OcclusionBuffer;
uniform sampler2D LightingBuffer;
uniform sampler2D DiffuseColorBuffer;

void main()
{
    compositeColor = AmbientTerm * texture(OcclusionBuffer, uv).rgb * texture(DiffuseColorBuffer, uv).rgb + texture(LightingBuffer, uv).rgb;
}