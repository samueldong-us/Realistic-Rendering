#version 330 core
#define Gamma 2.2
#define Bluriness 0.5
#define Samples 16

in vec2 uv;

out vec3 color;

uniform sampler2D DiffuseColorBuffer;
uniform sampler2D SpecularColorBuffer;
uniform sampler2D PositionBuffer;
uniform sampler2D NormalBuffer;
uniform sampler2D OcclusionBuffer;
uniform sampler2D LightingBuffer;
uniform sampler2D CompositeBuffer;

uniform mat4 View;
uniform mat4 PreviousView;
uniform mat4 Projection;

uniform float Exposure;
uniform int DisplayMode;

vec3 ToneMapping(vec3 hdrColor, float exposure);

void main()
{
    vec3 diffuseColor = texture(DiffuseColorBuffer, uv).rgb;
    vec3 specularColor = texture(SpecularColorBuffer, uv).rgb;
    vec3 position = texture(PositionBuffer, uv).xyz;
    vec3 normal = texture(NormalBuffer, uv).xyz;
    vec3 occlusionColor = texture(OcclusionBuffer, uv).rgb;
    vec3 lightingColor = texture(LightingBuffer, uv).rgb;
    vec3 compositeColor = texture(CompositeBuffer, uv).rgb;
    
    switch(DisplayMode)
    {
    case 0:
        vec4 worldSpacePosition = inverse(View) * vec4(position, 1.0);
        vec4 oldScreenSpacePosition = Projection * PreviousView * worldSpacePosition;
        oldScreenSpacePosition /= oldScreenSpacePosition.w;
        vec2 oldUV = (oldScreenSpacePosition.xy + 1.0) / 2.0;
        vec2 velocity = Bluriness * (uv - oldUV) / Samples;
        vec3 blurredColor = compositeColor;
        if (position != vec3(0.0))
        {
            vec2 offset;
            for (int i = 0; i < Samples; i++, offset += velocity)
            {
                blurredColor += texture(CompositeBuffer, uv + offset).rgb;
            }
            blurredColor /= Samples;
        }
        color = ToneMapping(blurredColor, Exposure);
        break;
    case 1:
        color = diffuseColor;
        break;
    case 2:
        color = specularColor;
        break;
    case 3:
        color = position;
        break;
    case 4:
        color = normal;
        break;
    case 5:
        color = ToneMapping(occlusionColor, Exposure);
        break;
    case 6:
        color = ToneMapping(lightingColor, Exposure);
        break;
    case 7:
        color = ToneMapping(compositeColor, Exposure);
        break;
    }
}

vec3 ToneMapping(vec3 hdrColor, float exposure)
{
    vec3 ldrColor = vec3(1.0) - exp(-hdrColor * exposure);
    return pow(ldrColor, vec3(1.0 / Gamma));
}