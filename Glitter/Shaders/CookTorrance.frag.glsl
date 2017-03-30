#version 330 core
#define Pi 3.1415926
#define Bias 0.5
#define NonConductor 0.04

in vec2 uv;

layout (location = 0) out vec3 lightingColor;

uniform sampler2D DiffuseColorBuffer;
uniform sampler2D SpecularColorBuffer;
uniform sampler2D PositionBuffer;
uniform sampler2D NormalBuffer;

uniform mat4 View;

uniform vec3 LightPosition;
uniform vec3 LightColor;
uniform float LightIntensity;

uniform samplerCube ShadowMap;
uniform float FarPlane;

float TrowbridgeReitzGGX(vec3 normal, vec3 halfway, float roughness);
float SchlickBeckmannGGXSmith(vec3 normal, vec3 view, vec3 light, float roughness);
vec3 FresnelSchlick(vec3 normal, vec3 view, vec3 baseReflectivity);

void main()
{
    vec3 diffuseColor = texture(DiffuseColorBuffer, uv).rgb;
    vec3 specularColor = texture(SpecularColorBuffer, uv).rgb;
    vec3 position = texture(PositionBuffer, uv).xyz;
    vec3 normal = texture(NormalBuffer, uv).xyz;

    vec3 worldPosition = (inverse(View) * vec4(position, 1.0)).xyz;
    vec3 lightToPosition = worldPosition - LightPosition;
    float depth = texture(ShadowMap, lightToPosition).r * FarPlane + Bias;
    if (length(lightToPosition) < depth)
    {
        float averageSpecular = (specularColor.r + specularColor.g + specularColor.b) / 3.0;
        float metalness = specularColor.r != specularColor.g ? averageSpecular : 0.0;
        float roughness = (1 - averageSpecular) * (1 - averageSpecular);

        vec3 viewDirection = normalize(-position);
        vec3 viewLightPosition = (View * vec4(LightPosition, 1.0)).xyz;
        vec3 lightDirection = normalize(viewLightPosition - position);
        vec3 halfwayDirection = normalize(viewDirection + lightDirection);

        vec3 ks = FresnelSchlick(halfwayDirection, viewDirection, mix(vec3(NonConductor), specularColor, metalness));
        vec3 kd = vec3(1.0) - ks;

        vec3 diffuseTerm = kd * diffuseColor / Pi;

        float specularNumerator = TrowbridgeReitzGGX(normal, halfwayDirection, roughness);
        specularNumerator *= SchlickBeckmannGGXSmith(normal, viewDirection, lightDirection, roughness);

        float incomingDot = max(dot(lightDirection, normal), 0.0);
        float outgoingDot = max(dot(viewDirection, normal), 0.0);
        float specularDenominator = max(4 * incomingDot * outgoingDot, Bias);

        vec3 specularTerm = ks * specularNumerator / specularDenominator;

        vec3 lightRadiance = LightColor * LightIntensity / dot(lightToPosition, lightToPosition);
        lightingColor = (diffuseTerm + specularTerm) * lightRadiance * incomingDot;
    }
}

float TrowbridgeReitzGGX(vec3 normal, vec3 halfway, float roughness)
{
    float roughnessSquared = roughness * roughness;
    float normalDotHalfway = max(dot(normal, halfway), 0.0);
    float normalDotHalfwaySquared = normalDotHalfway * normalDotHalfway;
    float denominatorTerm = normalDotHalfwaySquared * (roughnessSquared - 1.0) + 1.0;
    return roughnessSquared / (Pi * denominatorTerm * denominatorTerm);
}

float SchlickBeckmannGGX(vec3 normal, vec3 direction, float roughness)
{
    float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
    float normalDotDirection = max(dot(normal, direction), 0.0);
    return normalDotDirection / (normalDotDirection * (1.0 - k) + k);
}

float SchlickBeckmannGGXSmith(vec3 normal, vec3 view, vec3 light, float roughness)
{
    return SchlickBeckmannGGX(normal, view, roughness) * SchlickBeckmannGGX(normal, light, roughness);
}

vec3 FresnelSchlick(vec3 normal, vec3 view, vec3 baseReflectivity)
{
    float normalDotView = max(dot(normal, view), 0.0);
    return baseReflectivity + (1.0 - baseReflectivity) * pow(1.0 - normalDotView, 5.0);
}