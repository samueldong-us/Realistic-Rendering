#version 330 core

in vec3 position;
in vec2 uv;
in vec3 normal;
in vec3 tangent;
in vec3 bitangent;

layout (location = 0) out vec3 diffuseColor;
layout (location = 1) out vec3 specularColor;
layout (location = 2) out vec3 positionColor;
layout (location = 3) out vec3 normalColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_mask1;

uniform int screenWidth;
uniform int screenHeight;

void main()
{
    if (texture(texture_mask1, uv).r < 0.5)
    {
        discard;
    }
    diffuseColor = texture(texture_diffuse1, uv).rgb;
    specularColor = texture(texture_specular1, uv).rgb;
    positionColor = position;

    float du = texture(texture_normal1, uv + vec2(1.0 / screenWidth, 0.0)).r - texture(texture_normal1, uv - vec2(1.0 / screenWidth, 0.0)).r;
    float dv = texture(texture_normal1, uv + vec2(0.0, 1.0 / screenHeight)).r - texture(texture_normal1, uv - vec2(0.0, 1.0 / screenHeight)).r;
    vec3 modifiedTangent = normalize(tangent) + du * normalize(normal);
    vec3 modifiedBitangent = normalize(bitangent) + dv * normalize(normal);
    vec3 potentialNormal = cross(normalize(modifiedTangent), normalize(modifiedBitangent));
    if (dot(potentialNormal, normal) < 0.0)
    {
        normalColor = -potentialNormal;
    }
    else
    {
        normalColor = potentialNormal;
    }
}