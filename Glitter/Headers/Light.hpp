#pragma once
#include <string>

#include "shader.hpp"

namespace AdvancedRenderer
{
	class Light
	{
	public:
		float position[4];
		float color[3];
		float intensity;
		float attenuation;

		Light(const float x, const float y, const float z, const float w, const float r, const float g, const float b, const float strength, const float quadraticFalloff);

		void AttachTo(Shader shader, const std::string& light) const;
	};
}