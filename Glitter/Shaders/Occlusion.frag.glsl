#version 330 core
#define Pi 3.1415926
#define KernelSize 32
#define Bias 0.01
#define OcclusionStrength 2

in vec2 uv;

layout (location = 0) out vec3 occlusionColor;

uniform float Radius;
uniform vec3 Kernel[KernelSize];

uniform mat4 View;
uniform mat4 Projection;

uniform sampler2D DiffuseColorBuffer;
uniform sampler2D PositionBuffer;
uniform sampler2D NormalBuffer;

uniform samplerCube Environment;
uniform bool Directional;

void main()
{
    vec3 diffuseColor = texture(DiffuseColorBuffer, uv).rgb;
    vec3 position = texture(PositionBuffer, uv).xyz;
    vec3 normal = texture(NormalBuffer, uv).xyz;

    float solidAngle = 2.0 * Pi / KernelSize;
    vec3 accumulatedColor;
    float visible = 0.0;
    float total = 0.0;
    for (int i = 0; i < KernelSize; i++)
    {
        vec3 sampleOffset = mat3(View) * Radius * Kernel[i];
        if (dot(sampleOffset, normal) < 0.0)
        {
            sampleOffset *= -1;
        }
        vec3 sample = position + sampleOffset;
        vec4 projectedSample = Projection * vec4(sample, 1.0);
        projectedSample /= projectedSample.w;
        vec2 screenPosition = (projectedSample.xy + 1.0) / 2.0;
        float depth = texture(PositionBuffer, screenPosition).z;
        float cosine = dot(normalize(sampleOffset), normal);
        if (sample.z > depth - Bias || abs(depth - sample.z) > Radius)
        {
            vec3 radiance = vec3(1.0);
            if (Directional)
            {
                radiance = texture(Environment, inverse(mat3(View)) * sampleOffset).rgb;
            }
            accumulatedColor += radiance * 1.0 / Pi * cosine * solidAngle;
            visible += cosine;
        }
        total += cosine;
    }
    occlusionColor = accumulatedColor * pow(visible / total, OcclusionStrength);
}