#version 330 core

layout (location = 0) in vec3 _position;

out vec3 uv;

uniform mat4 View;
uniform mat4 Projection;

void main()
{
    vec4 position = Projection * mat4(mat3(View)) * vec4(_position, 1.0);
    gl_Position = position.xyww;
    uv = _position;
}