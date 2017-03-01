#version 330 core
layout (location = 0) in vec3 position;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 newPosition = projection * mat4(mat3(view)) * vec4(position, 1.0);
    gl_Position = newPosition.xyww;
    TexCoords = position;
}