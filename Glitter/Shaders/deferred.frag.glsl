#version 330 core
in vec2 TexCoords;

out vec4 color;

uniform sampler2D diffuseColor;
uniform sampler2D specularColor;
uniform sampler2D position;
uniform sampler2D normal;

void main()
{
    color = texture(diffuseColor, TexCoords);
}