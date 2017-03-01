#include "Light.hpp"
namespace AdvancedRenderer
{
	Light::Light(const float x, const float y, const float z, const float w, const float r, const float g, const float b, const float strength, const float quadraticFalloff)
	{
		position[0] = x;
		position[1] = y;
		position[2] = z;
		position[3] = w;

		color[0] = r;
		color[1] = g;
		color[2] = b;

		intensity = strength;
		attenuation = quadraticFalloff;
	}

	void Light::AttachTo(Shader shader, const std::string& light) const
	{
		glUniform4fv(glGetUniformLocation(shader.Program, (light + ".position").c_str()), 1, position);
		glUniform3fv(glGetUniformLocation(shader.Program, (light + ".color").c_str()), 1, color);
		glUniform1f(glGetUniformLocation(shader.Program, (light + ".intensity").c_str()), intensity);
		glUniform1f(glGetUniformLocation(shader.Program, (light + ".attenuation").c_str()), attenuation);
	}
}