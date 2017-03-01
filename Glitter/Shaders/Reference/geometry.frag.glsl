#version 330 core
layout(location = 0) out vec4 diffuseColor;
layout(location = 1) out vec4 specularColor;
layout(location = 2) out vec3 position;
layout(location = 3) out vec3 normal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_mask1;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec3 Tangent;
in vec3 BiTangent;

void main()
{
    if (texture(texture_mask1, TexCoords).r < 0.5)
    {
        discard;
    }
    diffuseColor = texture(texture_diffuse1, TexCoords);
    specularColor = texture(texture_specular1, TexCoords);
    position = FragPos;
    float uHeight = texture(texture_normal1, TexCoords + vec2(0.001, 0.0)).r - texture(texture_normal1, TexCoords - vec2(0.001, 0.0)).r;
    float vHeight = texture(texture_normal1, TexCoords + vec2(0.0, 0.001)).r - texture(texture_normal1, TexCoords - vec2(0.0, 0.001)).r;
    vec3 modifiedTangent = normalize(Tangent) + uHeight * normalize(Normal);
    vec3 modifiedBiTangent = normalize(BiTangent) + vHeight * normalize(Normal);
    vec3 potentialNormal = cross(normalize(modifiedTangent), normalize(modifiedBiTangent));
    if (dot(potentialNormal, Normal) < 0.0)
    {
        normal = -potentialNormal;
    }
    else
    {
        normal = potentialNormal;
    }
}