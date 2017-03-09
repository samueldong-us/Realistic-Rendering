#version 330 core

layout (location = 0) in vec2 _position;
layout (location = 1) in vec2 _uv;

out vec2 uv;

void main()
{
    gl_Position = vec4(_position, 0.0, 1.0);
    uv = _uv;
}