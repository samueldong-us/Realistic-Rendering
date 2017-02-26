#version 330 core
in vec3 TexCoords;

layout(location = 0) out vec4 diffuseColor;
layout(location = 1) out vec4 specularColor;
layout(location = 2) out vec3 position;
layout(location = 3) out vec3 normal;

uniform samplerCube skyBox;

void main()
{
    diffuseColor = texture(skyBox, TexCoords);
    specularColor = vec4(0.0, 0.0, 0.0, 0.0);
    position = vec3(0.0, 0.0, 0.0);
    normal = vec3(0.0, 0.0, 0.0);
}