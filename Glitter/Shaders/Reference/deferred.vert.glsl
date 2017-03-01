#version 330 core
in vec4 position;
in vec2 texCoords;

out vec2 TexCoords;

void main()
{
    gl_Position = position;
    TexCoords = texCoords;
}