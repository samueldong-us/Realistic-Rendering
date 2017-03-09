#version 330 core

layout (location = 0) in vec3 _position;
layout (location = 2) in vec2 _uv;

out vec3 position;
out vec2 uv;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
    gl_Position = Projection * View * Model * vec4(_position, 1.0);
    position = (Model * vec4(_position, 1.0)).xyz;
    uv = _uv;
}