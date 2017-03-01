#version 330 core
in vec2 TexCoords;

out vec4 color;

struct Light
{
    vec4 position;
    vec3 color;
    float intensity;
    float attenuation;
};

#define Number_Of_Lights 1

uniform float ambientFactor;
uniform float specularExponent;
uniform Light lights[Number_Of_Lights];

uniform mat4 view;

uniform sampler2D diffuseBuffer;
uniform sampler2D specularBuffer;
uniform sampler2D positionBuffer;
uniform sampler2D normalBuffer;

uniform int debug;

vec3 CalculateLight(Light light, vec3 diffuse, vec3 specular, vec3 normal, vec3 position);

void main()
{
    vec4 diffuse = texture(diffuseBuffer, TexCoords);
    vec4 specular = texture(specularBuffer, TexCoords);
    vec3 position = texture(positionBuffer, TexCoords).xyz;
    vec3 normal = texture(normalBuffer, TexCoords).xyz;
    switch(debug)
    {
        case 0:
            color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
            for (int i = 0; i < Number_Of_Lights; i++)
            {
                color.rgb += CalculateLight(lights[i], diffuse.rgb, specular.rgb, normal, position);
            }
            color.rgb += ambientFactor * diffuse.rgb;
            break;
        case 1:
            color = texture(diffuseBuffer, TexCoords);
            break;
        case 2:
            color = texture(specularBuffer, TexCoords);
            break;
        case 3:
            color = texture(positionBuffer, TexCoords);
            break;
        case 4:
            color = texture(normalBuffer, TexCoords);
            break;
    }
}

vec3 CalculateLight(Light light, vec3 diffuse, vec3 specular, vec3 normal, vec3 position)
{
    vec3 lightPosition = (view * light.position).xyz;
    vec3 lightDirection;
    if (light.position.w == 0.0)
    {
        lightDirection = -normalize(lightPosition);
    }
    else
    {
        lightDirection = normalize(lightPosition - position);
    }
    float diffuseFactor = max(dot(lightDirection, normal), 0.0);
    vec3 halfwayVector = normalize(lightDirection + -normalize(position));
    float specularFactor = pow(max(dot(halfwayVector, normal), 0.0), specularExponent);
    float attenuatedIntensity;
    if (light.attenuation == 0.0 || light.position.w == 0.0)
    {
        attenuatedIntensity = light.intensity;
    }
    else
    {
        attenuatedIntensity = light.intensity / (light.attenuation * dot(lightPosition - position, lightPosition - position));
    }
    return attenuatedIntensity * light.color * (diffuseFactor * diffuse + specularFactor * specular);
}