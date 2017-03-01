#version 330 core

layout (location = 0) in vec3 _position;
layout (location = 1) in vec3 _normal;
layout (location = 2) in vec2 _uv;
layout (location = 3) in vec3 _tangent;
layout (location = 4) in vec3 _bitangent;

out vec3 position;
out vec2 uv;
out vec3 normal;
out vec3 tangent;
out vec3 bitangent;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
    mat3 normalMatrix = transpose(inverse(mat3(View * Model)));
    vec4 viewSpacePosition = View * Model * vec4(_position, 1.0f);
    gl_Position = Projection * viewSpacePosition;
    position = viewSpacePosition.xyz;
    uv = _uv;
    normal = normalMatrix * _normal;
    tangent = normalMatrix * _tangent;
    bitangent = normalMatrix * _bitangent;
}