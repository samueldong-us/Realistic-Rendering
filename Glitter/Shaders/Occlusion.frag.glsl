#version 330 core
#define Pi 3.1415926
#define KernelSize 64
#define Bias 0.01

in vec2 uv;

layout (location = 0) out vec3 occlusionColor;

uniform float Radius;
uniform vec3 Kernel[KernelSize];

uniform mat4 View;
uniform mat4 Projection;

uniform sampler2D DiffuseColorBuffer;
uniform sampler2D PositionBuffer;
uniform sampler2D NormalBuffer;
uniform sampler2D DepthBuffer;

uniform samplerCube Environment;
uniform bool Directional;

void main()
{
    vec3 diffuseColor = texture(DiffuseColorBuffer, uv).rgb;
    vec3 position = texture(PositionBuffer, uv).xyz;
    vec3 normal = texture(NormalBuffer, uv).xyz;

    float solidAngle = 2 * Pi / KernelSize;
    for (int i = 0; i < KernelSize; i++)
    {
        vec3 sampleOffset = Radius * Kernel[i];
        if (dot(sampleOffset, normal) < 0.0)
        {
            sampleOffset *= -1;
        }
        vec3 sample = position + sampleOffset;
        vec4 projectedSample = Projection * vec4(sample, 1.0);
        projectedSample /= projectedSample.w;
        vec2 screenPosition = (projectedSample.xy + 1.0) / 2.0;
        float depth = texture(PositionBuffer, screenPosition).z;
        if (sample.z > depth - Bias || abs(depth - sample.z) > Radius)
        {
            vec3 radiance = vec3(1.0);
            if (Directional)
            {
                radiance = texture(Environment, inverse(mat3(View)) * sampleOffset).rgb;
            }
            float cosine = dot(normalize(sampleOffset), normal);
            occlusionColor += 1.0 / Pi * radiance * cosine * solidAngle;
        }
    }
}