#version 330 core
in vec2 TexCoords;

out vec4 color;

uniform sampler2D diffuseColor;
uniform sampler2D specularColor;
uniform sampler2D position;
uniform sampler2D normal;

uniform int debug;

void main()
{
    switch(debug)
    {
        case 0:
            color = texture(diffuseColor, TexCoords);
            break;
        case 1:
            color = texture(specularColor, TexCoords);
            break;
        case 2:
            color = texture(position, TexCoords);
            break;
        case 3:
            color = texture(normal, TexCoords);
    }
}